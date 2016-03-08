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

	stopMotors();
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

void stopMotors(void){
	for(int i = 0; i < 3; ++i){
		setMotorPwm(i, 0);
	}
}
