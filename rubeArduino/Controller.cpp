/*
 * Controller.cpp
 *
 *  Created on: 3 mar 2016
 *      Author: Mikael
 */

#include "Controller.h"

Controller::Controller(int motor){ //the constructor
	M = motor;
	lastTime = 0;
	Input = 0;
	Output = 0;
	Setpoint = 0;
	ITerm = 0;
	lastInput = 0;

	kp = 0;
	ki = 1;
	kd = 2;

	kup = 0;
	kui = 1;
	kud = 2;

	kdp = 0;
	kdi = 1;
	kdd = 2;

	SampleTime = 20;
	outMin = -35;
	outMax = 255;
}

//TODO make static?
void Controller::setAllControllers(Controller* controllers, float Pu, float Iu, float Du, float Pd, float Id, float Dd){
	controllers[1].SetTuningsUp(Pu,Iu,Du);
	controllers[2].SetTuningsUp(Pu,Iu,Du);
	controllers[3].SetTuningsUp(Pu,Iu,Du);

	controllers[1].SetTuningsDown(Pd,Id,Dd);
	controllers[2].SetTuningsDown(Pd,Id,Dd);
	controllers[3].SetTuningsDown(Pd,Id,Dd);
}

// usage: controllerA.Compute(1, posNow);
void Controller::Compute(int SetPt, long* posNow){
	Setpoint = SetPt;
	//boolean dir = true;
	unsigned long now = millis();
	int timeChange = (now - lastTime);
	if (timeChange >= SampleTime){
		/*Compute all the working error variables*/
		double error =  Setpoint - posNow[M] ;
		//NB here see what is actually up or down -change <to>
		if (error < 0) {
			kp = kup;
			ki = kui;
			kd = kud;
		}
		else {
			kp = kdp;
			ki = kdi;
			kd = kdd;
		}

		ITerm += (ki * error);
		if (ITerm > outMax) ITerm = outMax;
		else if (ITerm < outMin) ITerm = outMin;
		double dInput = (Input - lastInput);

		/*Compute Controller Output*/
		Output = kp * error + ITerm - kd * dInput;
		//Output=Output*-1;
		if (Output > outMax) Output = outMax;
		else if (Output < outMin) Output = outMin;

		/*Remember some variables for next time*/
		lastInput = Input;
		lastTime = now;
	}
}

void Controller::SetTuningsDown(double Kp, double Ki, double Kd){
	double SampleTimeInSec = ((double)SampleTime) / 1000;
	kdp = Kp;
	kdi = Ki * SampleTimeInSec;
	kdd = Kd / SampleTimeInSec;
}

void Controller::SetTuningsUp(double Kp, double Ki, double Kd){
	double SampleTimeInSec = ((double)SampleTime) / 1000;
	kup = Kp;
	kui = Ki * SampleTimeInSec;
	kud = Kd / SampleTimeInSec;
}

void Controller::SetSampleTime(int NewSampleTime){
	if (NewSampleTime > 0)
	{
		double ratio  = (double)NewSampleTime
				/ (double)SampleTime;
		ki *= ratio;
		kd /= ratio;
		SampleTime = (unsigned long)NewSampleTime;
	}
}

void Controller::SetOutputLimits(double Min, double Max){
	if (Min > Max) return;
	outMin = Min;
	outMax = Max;

	if (Output > outMax) Output = outMax;
	else if (Output < outMin) Output = outMin;

	if (ITerm > outMax) ITerm = outMax;
	else if (ITerm < outMin) ITerm = outMin;
}

