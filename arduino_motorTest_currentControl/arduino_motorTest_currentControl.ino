#include "Arduino.h"

const int pin_feedbackA1 = 0;
const int pin_feedbackA2 = 1;
const int pin_feedbackB1 = 2;
const int pin_feedbackB2 = 3;

const int pin_pot = 4;

const int pin_in1_A1 = 23;
const int pin_in2_A1 = 22;
const int pin_in1_A2 = 21;
const int pin_in2_A2 = 20;
const int pin_in1_B1 = 10;
const int pin_in2_B1 = 9;
const int pin_in1_B2 = 6;
const int pin_in2_B2 = 5;

uint8_t pin_in1[] = {pin_in1_A1, pin_in1_A2, pin_in1_B1, pin_in1_B2};
uint8_t pin_in2[] = {pin_in2_A1, pin_in2_A2, pin_in2_B1, pin_in2_B2};

const int pin_disable_motors =  2;
const int pin_statusFlag_A1 =  3;
const int pin_statusFlag_A2 =  4;
const int pin_statusFlag_B1 =  11;
const int pin_statusFlag_B2 =  12;

float lastPwm[] = {0,0,0};
int maxAnalogWrite = 255;// 1 << analog_write_res;
int32_t motorPwmFreq = 15000;
volatile float timeStepcurrentmilliAmpsControll = 1/(float)motorPwmFreq;

float ramp = 0.01;
float tempdelta = 0.0002;
volatile uint32_t i = 0;
const int tempPin = 8;
volatile int tempVal = 0;
volatile int temptemp = 0;

// U=R*I, R = 270, I =  0.0024*currentmilliAmps
// U = 270*0.0024*currentmilliAmps
// U = 0,648*currentmilliAmps
float lsbVolt = 0.0012;
volatile int32_t multiplier = round(1000*(1/(float)0.648)*lsbVolt);

volatile int32_t currentmilliAmpsRef = 0;

uint32_t lastLoopStart;
float timeStep;
uint32_t plotTime = millis();
volatile int32_t currentmilliAmpsIntergral = 0;
volatile int32_t val = 0;

volatile int32_t pidP = (float)0.2*(float)motorPwmFreq;
volatile int32_t pidI = 250;
volatile int32_t currentmilliAmps = 0;

//Constants for bitmasks for FTM
#define FTMx_CnSC_CHIE (1<<6) // per UG p 702
#define FTMx_CnSC_MSB (1 <<4) //
#define FTMx_CnSC_CHF (1<<7) // per UG pg 702

void initTimerInterupt(){
	pinMode(tempPin, OUTPUT);
	  FTM0_SC |= FTM_SC_TOIE; //overflow interupt
	  NVIC_ENABLE_IRQ(IRQ_FTM0); // enable the interrupt
}

/*
 * motorPwm is in range -1000 to 1000
 */
void setMotorPwm(int motorIndex, int32_t motorPwm){
	int32_t maxPwm = 1000;
	if (motorPwm > maxPwm){
		motorPwm = maxPwm;
	}else if(motorPwm < -maxPwm){
		motorPwm = -maxPwm;
	}

	int32_t absPwm = motorPwm;
	if (absPwm < 0) absPwm = - absPwm;

	if (motorIndex == 0){
		temptemp = (maxAnalogWrite*(maxPwm-absPwm))/maxPwm;
	}

	if (motorPwm == 0){
		analogWrite(pin_in1[motorIndex], 0);
		analogWrite(pin_in2[motorIndex], 0);
	}else if (motorPwm < 0){
		analogWrite(pin_in1[motorIndex], maxAnalogWrite);
		analogWrite(pin_in2[motorIndex], (maxAnalogWrite*(maxPwm-absPwm))/maxPwm);
	}else{
		analogWrite(pin_in1[motorIndex], (maxAnalogWrite*(maxPwm-absPwm))/maxPwm);
		analogWrite(pin_in2[motorIndex], maxAnalogWrite);
	}
}

void setMotorPwms(float* motorPwms){
	for(int i = 0; i < 3; ++i){
		setMotorPwm(i, motorPwms[i]);
	}
}

void stopMotors(void){
	for(int i = 0; i < 3; ++i){
		setMotorPwm(i, 0);
	}
}

void initMotors(void){
	pinMode(pin_disable_motors,OUTPUT);
	digitalWrite(pin_disable_motors, LOW);

	pinMode(pin_in1_A1,OUTPUT);
	pinMode(pin_in2_A1,OUTPUT);
	pinMode(pin_statusFlag_A1,INPUT);

	pinMode(pin_in1_A2,OUTPUT);
	pinMode(pin_in2_A2,OUTPUT);
	pinMode(pin_statusFlag_A2,INPUT);

	pinMode(pin_in1_B1,OUTPUT);
	pinMode(pin_in2_B1,OUTPUT);
	pinMode(pin_statusFlag_B1,INPUT);

	pinMode(pin_in1_B2,OUTPUT);
	pinMode(pin_in2_B2,OUTPUT);
	pinMode(pin_statusFlag_B2,INPUT);

	analogWriteFrequency(pin_in1_A1, motorPwmFreq);//affects all pwm pins on the same timer

	stopMotors();
}

void setup(){
	Serial.begin(9600);
	initMotors();
	//	setMotorPwm(0, 0.05);
	//	setMotorPwm(1, 0.7);
	//	setMotorPwm(2, 0.7);
	//	setMotorPwm(3, 0.7);

	analogReference(INTERNAL);

	cli();
	initTimerInterupt();
	sei();

	Serial.println("init done");
}

void loop(){
//	delay(10);
	uint32_t loopStart = millis();
	timeStep = 0.001*(loopStart - lastLoopStart);
	ramp+=tempdelta;

	if (ramp > 1){
		tempdelta = - tempdelta;
		ramp = 1;
	}else if(ramp < 0){
		tempdelta = - tempdelta;
		ramp = 0;
	}
//	val = ramp;

	float ref = (1/(float)1023) * analogRead(pin_pot);
	ref = 2*ref - 1;
	//	if(fabs(val) <0.05) val = 0;
	float currentmilliAmpsMax = 500;
	currentmilliAmpsRef = currentmilliAmpsMax*ref;
//	currentmilliAmpsRef = 700;
//	delay(30);
//	currentmilliAmpsRef = 0;
//	delay(200);

	if (loopStart > plotTime){
		Serial.print("timeStep us: ");
		Serial.print(1000*timeStep);
		Serial.print(", ref: ");
		Serial.print(currentmilliAmpsRef);
		Serial.print(" currentmilliAmps: ");
		Serial.print(currentmilliAmps);
		Serial.print(" integral: ");
		Serial.print(currentmilliAmpsIntergral/motorPwmFreq);
		Serial.print(", sending: ");
		Serial.println(val);
//		Serial.print(", sending: ");
//		Serial.println(temptemp);
		plotTime += 100;
	}
	lastLoopStart = loopStart;
}
void ftm0_isr(void){
	digitalWrite(tempPin,1);
	  FTM0_SC &=  ~FTM_SC_TOF;


	  	currentmilliAmps = multiplier*analogRead(pin_feedbackA1);
	  	if (val < 0) currentmilliAmps = -currentmilliAmps;
	  	/*
	  	 * 0.068V = 60
	  	 * 0.734V  = 620
	  	 * lsb = 0.0012 V
	  	 * offset = 0V
	  	 *
	  	 */

	  	int32_t currentmilliAmpsError = currentmilliAmpsRef - currentmilliAmps;
	  	currentmilliAmpsIntergral += currentmilliAmpsError*pidI;
	  	int32_t integralMax = 1000*motorPwmFreq;
	  	if(currentmilliAmpsIntergral > integralMax) currentmilliAmpsIntergral = integralMax;
	  	if(currentmilliAmpsIntergral < -integralMax) currentmilliAmpsIntergral = -integralMax;

	  	val = (pidP*currentmilliAmpsError + currentmilliAmpsIntergral)/motorPwmFreq;

	  	setMotorPwm(0, val);
//	  	setMotorPwm(1, val);
//	  	setMotorPwm(2, val);
//	  	setMotorPwm(3, val);

	  	digitalWrite(tempPin,0);
}



