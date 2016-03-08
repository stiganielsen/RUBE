#include "Arduino.h"
int pwmPinA=3;
int directionPin1A=8;
int directionPin2A=7;

int pwmPinB=4;
int directionPin1B=10;
int directionPin2B=9;

int pwmPinC=5;
int directionPin1C=12;
int directionPin2C=11;

uint8_t pwmPin[] = {3,4,5};
uint8_t directionPin1[] = {8, 10, 12};
uint8_t directionPin2[] = {7, 9, 11};
float lastPwm[] = {0,0,0};
int maxAnalogWrite = 255;// 1 << analog_write_res;

void initMotors(void){
	pinMode(pwmPinA,OUTPUT);
	pinMode(directionPin1A,OUTPUT);
	pinMode(directionPin2A,OUTPUT);

	pinMode(pwmPinB,OUTPUT);
	pinMode(directionPin1B,OUTPUT);
	pinMode(directionPin2B,OUTPUT);

	pinMode(pwmPinC,OUTPUT);
	pinMode(directionPin1C,OUTPUT);
	pinMode(directionPin2C,OUTPUT);

	setMotorPwms(0, 0, 0);

//	//Stop and set motor direction to start one way.
//	analogWrite(pwmPinA,0);
//	analogWrite(pwmPinB,0);
//	analogWrite(pwmPinC,0);
//	digitalWrite(directionPin1A,HIGH);
//	digitalWrite(directionPin1B,HIGH);
//	digitalWrite(directionPin1C,HIGH);
//	digitalWrite(directionPin2A,LOW);
//	digitalWrite(directionPin2B,LOW);
//	digitalWrite(directionPin2C,LOW);

}

void setMotorPwm(int motorIndex, float motorPwm){
	if (motorPwm*lastPwm[motorIndex] > 0){
		//keep going in same direction, so nothing to do
	}else if (motorPwm==0) {
		digitalWrite(directionPin1[motorIndex],HIGH);
		digitalWrite (directionPin2[motorIndex],HIGH);
	}else if(motorPwm>0){
		digitalWrite(directionPin1[motorIndex],HIGH);
		digitalWrite (directionPin2[motorIndex],LOW);
	}
	else{
		digitalWrite(directionPin1[motorIndex],LOW);
		digitalWrite (directionPin2[motorIndex],HIGH);
	}
	analogWrite(pwmPin[motorIndex], abs((int)round(maxAnalogWrite*motorPwm)));
	lastPwm[motorIndex]=motorPwm;
}

void setMotorPwms(float* motorPwms){
	for(int i = 0; i < 3; ++i){
		setMotorPwm(i, motorPwms[i]);
	}
}

//void motor(int motornumber,int spd){
//	unsigned int directionPin1 = 0;
//	unsigned int directionPin2 = 0;
//	int pwmPin = 0;
//
//	switch(motornumber) {
//	case 0:
//		directionPin1 = directionPin1A;
//		directionPin2 = directionPin2A;
//		pwmPin = pwmPinA;
//
//		break;
//	case 1:
//		directionPin1 = directionPin1B;
//		directionPin2 = directionPin2B;
//		pwmPin = pwmPinB;
//		break;
//	case 2:
//		directionPin1 = directionPin1C;
//		directionPin2 = directionPin2C;
//		pwmPin = pwmPinC;
//		break;
//	default:
//		return;
//		break;
//	}
//
//	if (spd==0) {  digitalWrite(directionPin1,HIGH); digitalWrite (directionPin2,HIGH);analogWrite(pwmPin, abs(spd));}
//	else if(spd>0){digitalWrite(directionPin1,HIGH); digitalWrite (directionPin2,LOW); analogWrite(pwmPin, abs(spd));}
//	else{		   digitalWrite(directionPin1,LOW);  digitalWrite (directionPin2,HIGH);analogWrite(pwmPin, abs(spd));}
//}

