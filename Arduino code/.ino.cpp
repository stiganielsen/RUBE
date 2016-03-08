//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2016-03-06 14:16:29

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
void getLineVel2xyzVelMatrix(float* resultMatrixPointer);
void getXyzVel2lineVelMatrix(float* LineVel2xyzVelMatrix, float* resultMatrixPointer);
void getRubePosition(float* posPointer);
void getLineLengths(float* lengthsPointer);
void initMotors(void);
void setMotorPwm(int motorIndex, float motorPwm);
void setMotorPwms(float motorPwm1, float motorPwm2, float motorPwm3);
void setup();
void loop();
void  setAllPIDs();
void playSound(int freq, int duration);
void soundStart(void);
void soundFinished(void);
void soundError(void);

#include "rube.ino"

#include "buttons.ino"
#include "communication.ino"
#include "kinematics.ino"
#include "motor.ino"
#include "sounds.ino"
