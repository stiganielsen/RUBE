//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2016-03-08 03:15:59

#include "Arduino.h"
#include "Arduino.h"
#include "Arduino.h"
#include "Arduino.h"
#include "Arduino.h"
#include "Controller.h"
#include <Encoder.h>
void initButtons();
boolean isButtonAPressed(void);
boolean isButtonBPressed(void);
boolean isButtonCPressed(void);
void initCommunication(void);
void sendBuffer(void);
long parseInt32();
void serialEvent();
void initKinematics(void);
void CalculateLineOPQ(float* posPointer);
void deltaLineDeltaXyz(float* xyzRube, float* dLengthDt, float* resultVector);
void xyzForce2lineForce(float* xyzRube, float* xyzForce, float* resultVector);
void deltaXyzDeltaLine(float* xyzRube, float* dXyzDt, float* resultVector);
void line2xyzRube(float* lineLengths,  float* resultVector);
void initMotors(void);
void setMotorPwm(int motorIndex, float motorPwm);
void setMotorPwms(float* motorPwms);
void stopMotors(void);
void setup();
void loop();
void playSound(int freq, int duration);
void soundStart(void);
void soundFinished(void);
void soundError(void);

#include "rubeArduino.ino"

#include "buttons.ino"
#include "communication.ino"
#include "kinematics.ino"
#include "motor.ino"
#include "sounds.ino"
