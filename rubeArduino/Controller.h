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
	double output;
	float lastError;
	float iTerm;
	float iTermMax;
	float iTermMin;
	float kp, ki, kd;
	float outMin, outMax;
	Controller(float P, float I, float D);
	//void setAllControllers(Controller* controllers, float Pu, float Iu, float Du, float Pd, float Id, float Dd);
//	void Compute(int SetPt, long* posNow);
	void update(float error, float timeStep);
//	void SetTuningsDown(double Kp, double Ki, double Kd);
//	void SetTuningsUp(double Kp, double Ki, double Kd);
//	void SetSampleTime(int NewSampleTime);
//	void SetOutputLimits(double Min, double Max);
};

#endif /* CONTROLLER_H_ */
