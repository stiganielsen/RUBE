/*
 * Controller.h
 *
 *  Created on: 3 mar 2016
 *      Author: Mikael
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <Arduino.h>


class Controller {
public:
	int M; // a variable
	unsigned long lastTime;
	double Input;
	double output;
	double Setpoint;
	double ITerm, lastInput;
	float lastError;
	float iTermMax;
	float iTermMin;
	double kp, ki, kd, kup, kui, kud, kdp, kdi, kdd;
	int SampleTime = 20; //1 sec
	double outMin, outMax;
	Controller(int motor);
	//void setAllControllers(Controller* controllers, float Pu, float Iu, float Du, float Pd, float Id, float Dd);
//	void Compute(int SetPt, long* posNow);
	void update(float error, float timeStep);
//	void SetTuningsDown(double Kp, double Ki, double Kd);
//	void SetTuningsUp(double Kp, double Ki, double Kd);
//	void SetSampleTime(int NewSampleTime);
//	void SetOutputLimits(double Min, double Max);
};

#endif /* CONTROLLER_H_ */
