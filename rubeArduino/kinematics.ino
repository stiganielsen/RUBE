#include "Arduino.h"

// -------- measured values --------
//HEIGHT(s)
float heightO=3166;//16000;
float heightP = 3166;
float heightQ = 3166;
//DISTANCES BETWEEN TOP PTS
float distOP= 3850;//ljusg15570;//crf628;//origo -> første  (o) O------P  (x)
float distPQ= 3752;//ljusg15980;//crf600;//første til anden      \  R /
float distOQ= 3300;//ljusg25640;//crf437;//anden til origo         \  /
// 						                                           (y)    Q

// -------- constants and variables generated during runtime --------
float deltaXyzDeltaLineMatrix[3][3];
float lineAttachment[3][3];

void initKinematics(void){
	//project distOP, distPQ, distOQ onto x-y-plane using pythagoras theorem
	float heightDiffOP = heightO - heightP;
	float heightDiffOQ = heightO - heightQ;
	float heightDiffPQ = heightP - heightQ;
	float distOPproj = sqrt(distOP*distOP - heightDiffOP*heightDiffOP);
	float distOQproj = sqrt(distOQ*distOQ - heightDiffOQ*heightDiffOQ);
	float distPQproj = sqrt(distPQ*distPQ - heightDiffPQ*heightDiffPQ);

	//law of cosines:
	float angleQOP = acos((distOPproj*distOPproj + distOQproj*distOQproj - distPQproj*distPQproj)/(2*distOPproj*distOQproj));
	float angleQOY = M_PI/2 - angleQOP;

	lineAttachment[0][0] = 0;
	lineAttachment[0][1] = 0;
	lineAttachment[0][2] = heightO;

	lineAttachment[1][0] = 0;
	lineAttachment[1][1] = distOPproj;
	lineAttachment[1][2] = heightP;

	lineAttachment[2][0] = sin(angleQOY)*distOQproj;
	lineAttachment[2][1] = cos(angleQOY)*distOQproj;
	lineAttachment[2][2] = heightQ;
}

/*
 * convert a force in xyz to equivalent force in each line
 */
void xyzForce2lineForce(float* xyzRube, float* xyzForce, float* resultVector){
	deltaXyzDeltaLine(xyzRube, xyzForce, resultVector); //TODO check if correct
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
		float lineLength = norm(line);
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

void xyzRube2line(float* xyzRube,  float* resultVector){
	float xyzLine[3];
	for( int i = 0; i < 3 ; ++i){ // for each line
		for( int j = 0; j < 3 ; ++j){//for xyz
			xyzLine[j] = lineAttachment[i][j] - xyzRube[j];
		}
		resultVector[i] = norm(xyzLine);
	}
}

void line2xyzRube(float* lineLength,  float* resultVector){
	float lineAttachment1Temp[3] = {lineAttachment[0][0],lineAttachment[0][1],lineAttachment[0][2]};
	float lineAttachment2Temp[3] = {lineAttachment[1][0],lineAttachment[1][1],lineAttachment[1][2]};
	float lineAttachment3Temp[3] = {lineAttachment[2][0],lineAttachment[2][1],lineAttachment[2][2]};

	float xyzRubePossibility1[3];
	float xyzRubePossibility2[3];

	threeCirclesIntersection(lineAttachment1Temp, lineAttachment2Temp, lineAttachment3Temp, lineLength[0], lineLength[1], lineLength[2], xyzRubePossibility1, xyzRubePossibility2);
	if (xyzRubePossibility1[2] < xyzRubePossibility2[2]){
		resultVector[0] = xyzRubePossibility1[0];
		resultVector[1] = xyzRubePossibility1[1];
		resultVector[2] = xyzRubePossibility1[2];
	}else{
		resultVector[0] = xyzRubePossibility2[0];
		resultVector[1] = xyzRubePossibility2[1];
		resultVector[2] = xyzRubePossibility2[2];
	}
}

/* Find the intersection of three spheres
 * A,B,C are the centers, r1,r2,r3 are the radii
 * Implementaton based on Wikipedia Trilateration article.
 * http://stackoverflow.com/questions/1406375/finding-intersection-points-between-3-spheres
 * returns the two intersections points in resultVector1 & resultVector2
 */
int threeCirclesIntersection(float* A, float* B, float* C, float r1, float r2, float r3, float* resultVector1, float* resultVector2){
	float AB[3];
	float e_x[3];
	float CA[3];
	float temp3[3];
	float e_y[3];
	float e_z[3];

	AB[0] = B[0]-A[0];
	AB[1] = B[1]-A[1];
	AB[2] = B[2]-A[2];

	float absAB = norm(AB);

	e_x[0] = AB[0]/absAB;
	e_x[1] = AB[1]/absAB;
	e_x[2] = AB[2]/absAB;

	CA[0] = C[0]-A[0];
	CA[1] = C[1]-A[1];
	CA[2] = C[2]-A[2];

	float i = dotProduct(e_x,CA);

	temp3[0] = CA[0] - i*e_x[0];
	temp3[1] = CA[1] - i*e_x[1];
	temp3[2] = CA[2] - i*e_x[2];

	float absTemp3 = norm(temp3);

	e_y[0] = temp3[0]/absTemp3;
	e_y[1] = temp3[1]/absTemp3;
	e_y[2] = temp3[2]/absTemp3;

	crossProduct(e_x,e_y, e_z);

	float d = norm(AB);

	float j = dotProduct(e_y,CA);

	float x = (r1*r1 - r2*r2 + d*d) / (2*d);
	float y = (r1*r1 - r3*r3 -2*i*x + i*i + j*j) / (2*j);
	float temp4 = r1*r1 - x*x - y*y;
	if (temp4 < 0){
		return 1; //The three spheres do not intersect!
	}else{
		float z = sqrt(temp4);
		resultVector1[0] = A[0] + x*e_x[0] + y*e_y[0] + z*e_z[0];
		resultVector1[1] = A[1] + x*e_x[1] + y*e_y[1] + z*e_z[1];
		resultVector1[2] = A[2] + x*e_x[2] + y*e_y[2] + z*e_z[2];

		resultVector2[0] = A[0] + x*e_x[0] + y*e_y[0] - z*e_z[0];
		resultVector2[1] = A[1] + x*e_x[1] + y*e_y[1] - z*e_z[1];
		resultVector2[2] = A[2] + x*e_x[2] + y*e_y[2] - z*e_z[2];
		return 0;
	}
}

float norm(float* a){
	return sqrt(a[1]*a[1] + a[2]*a[2] + a[3]*a[3]);
}

float dotProduct(float* a, float* b){
	return a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
}

void crossProduct(float* a, float* b, float* resultVector){
	resultVector[1] = a[2]*b[3]-a[3]*b[2];
	resultVector[2] = a[3]*b[1]-a[1]*b[3];
	resultVector[3] = a[1]*b[2]-a[2]*b[1];
}
