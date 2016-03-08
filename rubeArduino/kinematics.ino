#include "Arduino.h"

//____________________________MeasureValues
 //HEIGHT(s)
float height=3166;//16000; //currently the rube must be mounted so that the points are in the same height.
//DISTANCES BETWEEN TOP PTS
float distOP= 3850;//ljusg15570;//crf628;//origo -> første  (o) O------P  (x)
float distPQ= 3752;//ljusg15980;//crf600;//første til anden      \  R /
float distOQ= 3300;//ljusg25640;//crf437;//anden til origo         \  /
//DISTANCES TO RUBE                                           (y)   Q
float distOR =2600;//20750;
float distPR =2870;//16450; //1444 er højden fra gulv sjuss
float distQR =2456;//21520;
//____________________________MeasureValues
int lineO=distOR;
int lineP=distPR;
int lineQ=distQR;
long Qx,Qy,Qz;

float gox=0;
float goy=0;
float goz=0;

int Rx=0;
int Ry=0;
int Rz=0;

//-------vilse-style-------
float deltaXyzDeltaLineMatrix[3][3];

float lineAttachment[3][3];

void initKinematics(void){
	//first finding the XY position of point Q
	float r1=distOQ;
	float r2=distPQ;
	float d=distOP;
	Qx=((d*d)-(r2*r2)+(r1*r1))/(2*d);
	Qy=((1/d) * sqrt((-d+r2-r1)*(-d-r2+r1)*(-d+r2+r1)*(d+r2+r1)))/2;
	Qz =height;
	//now finding the pointR based on the three other points OPQ and the distances from these to pointR
	r1=distOR;
	r2=distPR;
	int r3=distQR;
	d=distOP;
	float i=Qx; //pointQ.x
	float j=Qy; //pointQ.y

	Rx=int(( ( (r1*r1) - (r2*r2) + (d*d) ) /  (2*d) ));
	Ry=int(( ( (r1*r1) - (r3*r3) + (i*i) + (j*j) ) / (2*j) ) - ( (i/j)*Rx )); // ((r1*r1)-(r3*r3)-(xc*xc)+( (xc-i)*(xc-i) )+(j*j) / (2*j));
	Rz=int( sqrt( (r1*r1) - (Rx*Rx) - (Ry*Ry) ))  ;
	Rz=height-Rz;

	//-------vilse-style-------
	for( int i = 0; i < 3 ; ++i){ // for each line
		for( int j = 0; j < 3 ; ++j){//for xyz
			lineAttachment[i][j] = 0; //TODO
		}
	}

}

void CalculateLineOPQ(float* posPointer){
	gox=posPointer[0];
	goy=posPointer[1];
	goz=posPointer[2];
	float d= distOP;
	float i= Qx;
	float j= Qy;
	float h=height;
	lineO =int(sqrt((gox*gox)+(goy*goy)+((goz-h)*(goz-h))));
	lineP =int(sqrt(((gox-d)*(gox-d))+(goy*goy)+((goz-h)*(goz-h)))) ;
	lineQ =int(sqrt(((gox-i)*(gox-i))+((goy-j)*(goy-j))+((goz-h)*(goz-h)))) ;
}

//TODO
/*
 * inverse of deltaXyzDeltaLine
 * [3x1]  =      [3 x 3]         * [3 x 1]
 * xyzVel = lineVel2xyzVelMatrix * lineVel
 * xyzAcc = lineVel2xyzVelMatrix * lineAcc
 */
void deltaLineDeltaXyz(float* xyzRube, float* dLengthDt, float* resultVector){

}
/*
 * convert a force in xyz to equivalent force in each line
 */
void xyzForce2lineForce(float* xyzRube, float* xyzForce, float* resultVector){
	deltaXyzDeltaLine(xyzRube, xyzForce, resultVector); //TODO check if correct
}
/*
 * inverse of deltaLineDeltaXyz
 * [3x1]   =      [3 x 3]         * [3 x 1]
 * lineVel = xyzVel2lineVelMatrix *  xyzVel
 * lineAcc = xyzVel2lineVelMatrix *  xyzAcc
 */
void deltaXyzDeltaLine(float* xyzRube, float* dXyzDt, float* resultVector){
	float line[3];

	//calculate new matrix  TODO: only if necessary
	for( int i = 0; i < 3 ; ++i){ // for each line
		//find  xyz component of each line
		for( int j = 0; j < 3 ; ++j){//for xyz
			line[j] = lineAttachment[i][j]  - xyzRube[j];
		}

		//project cartesian xyz velocity on line,
		float lineLength = sqrt(line[0]*line[0] + line[1]*line[1] + line[2]*line[2]);
		for( int j = 0; j < 3 ; ++j){//for each row of matrix
			deltaXyzDeltaLineMatrix[i][j] = line[j]/lineLength;
		}
	}

	//find lineVels
	for( int i = 0; i < 3 ; ++i){ // for each line
		for( int j = 0; j < 3 ; ++j){//for xyz
			resultVector[i] = resultVector[i] + deltaXyzDeltaLineMatrix[i][j]*dXyzDt[j];
		}
	}
}

//TODO
void line2xyzRube(float* lineLengths,  float* resultVector){
	resultVector[0] = 0;
	resultVector[1] = 0;
	resultVector[2] = 0;
}
