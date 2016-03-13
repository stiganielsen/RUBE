#include "Controller.h"
#include <Encoder.h>

extern float lineAttachment[3][3];
float lineLength[3];
float lineVelRube[3];
float xyzRube[3];
float xyzVelRube[3];

uint32_t lastLoopStart;

float velLowpassTimeConstant = 0.05;

//------ position controller ---------
float waypointXYZ[3][2]; //TODO list of waypoints
float waypointSpeed[2]= { 0.1, 0.1};
float instantaneousXyzTarget[3];
float speedTarget = 0.1; //m/s
float maxAcc = 0.1;
float posControllerP = 1;
float posControllerI = 0;
float posControllerD = 0;

Controller controllerX = Controller(posControllerP, posControllerI, posControllerD);
Controller controllerY = Controller(posControllerP, posControllerI, posControllerD);
Controller controllerZ = Controller(posControllerP, posControllerI, posControllerD);

//------ velocity controller ---------
float xyzVelTarget[3]={0, 0, 0};
float velControllerP = 1;
float velControllerI = 0;
float velControllerD = 0;

Controller controllerXvel = Controller(velControllerP, velControllerI, velControllerD);
Controller controllerYvel = Controller(velControllerP, velControllerI, velControllerD);
Controller controllerZvel = Controller(velControllerP, velControllerI, velControllerD);

// force calculations
float mass = 2; //kg
float gravitAcc = 9.82; //m/s^2
float spoolRadius[3]={ 0.1, 0.1, 0.1};
float frictionForce = 0.05;
float motorKv = 4000; //rpm/Volt
float motorKt = 9.5/motorKv; //Nm/A
float motorR = 0.1; //Ohm
float batteryVoltage = 12; //Volt
float motorPwm[3]; // 0 to 1

//Controller controllerA = Controller(0);
//Controller controllerB = Controller(1);
//Controller controllerC = Controller(2);

Encoder encA(29,28);
Encoder encB(30,27);
Encoder encC(26,31);
float lengthPerEncStep = 0.001;

////PID variables
//float Pu =2;
//float Iu =1;
//float Du =0.5;

//int cnt=0;
int speakerPin = 24;

//vector math
#define vectorAdd(a,b, result) 		result[0]= a[0]+b[0]; result[1]= a[1]+b[1]; result[2]= a[2]+b[2]
#define vectorSubtract(a,b, result) result[0]= a[0]-b[0]; result[1]= a[1]-b[1]; result[2]= a[2]-b[2]
#define vectorDotProduct(a,b, result) result[0]= a[0]*b[0]; result[1]= a[1]*b[1]; result[2]= a[2]*b[2]
#define vectorScalarMultiplication(scalar,vector, result) result[0]= scalar*vector[0]; result[1]= scalar*vector[1]; result[2]= scalar*vector[2]

void setup(){
	pinMode(speakerPin,OUTPUT); //init speaker
	initButtons();
	initMotors();
	initKinematics();
	initCommunication();

	soundStart();

	//TODO make communication event based using github-repo instead of standard write/read
	//TODO calibrate OPQ from given rubeXyz, measured line-angles
	//TODO estimate payload weight
	//TODO multiple ways of calibrating?

	//Tension slack on lines
	while(isButtonAPressed() || isButtonBPressed() || isButtonCPressed()){
		if(isButtonAPressed()){
			setMotorPwm(0, 0.4);
		}else{
			setMotorPwm(0, -0.2);
		}

		if(isButtonBPressed()){
			setMotorPwm(1, 0.4);
		}else{
			setMotorPwm(1, -0.2);
		}

		if(isButtonCPressed()){
			setMotorPwm(2, 0.4);
		}else{
			setMotorPwm(2, -0.2);
		}
	}
	stopMotors();

//	//DISTANCES TO RUBE
//	float distOR =2600;//20750;
//	float distPR =2870;//16450; //1444 er højden fra gulv sjuss
//	float distQR =2456;//21520;
	//TODO calibrate xyz with distance sensor
	//assuming rube is centered and on the floor, find average x and y of lineAttachements
	xyzRube[0] = (lineAttachment[0][0] + lineAttachment[1][0] + lineAttachment[2][0])/3;
	xyzRube[1] = (lineAttachment[0][1] + lineAttachment[1][1] + lineAttachment[2][1])/3;
	xyzRube[2] = 0;

	xyzRube2line(xyzRube,  lineLength);

	encA.write(lineLength[0]);
	encB.write(lineLength[1]);
	encC.write(lineLength[2]);

	waypointXYZ[0][0] = xyzRube[0];
	waypointXYZ[1][0] = xyzRube[1];
	waypointXYZ[2][0] = xyzRube[2];

	waypointXYZ[0][1] = xyzRube[0];
	waypointXYZ[1][1] = xyzRube[1];
	waypointXYZ[2][1] = xyzRube[2] + 100;


//	printState();
}

//void printState(void){
//	wirelessSerial.print("rube X:"); wirelessSerial.print(xyzRube[0]);
//	wirelessSerial.print(" Y:"); wirelessSerial.print(xyzRube[1]);
//	wirelessSerial.print(" Z:"); wirelessSerial.println(xyzRube[2]);
//
//	wirelessSerial.print("line A:"); wirelessSerial.print(lineLength[0]);
//	wirelessSerial.print(" B:"); wirelessSerial.print(lineLength[1]);
//	wirelessSerial.print(" C:"); wirelessSerial.println(lineLength[2]);
//}


void loop(){
	uint32_t loopStart = millis();
	float timeStep = 0.001*(loopStart - lastLoopStart);
	//TODO moving average filter
	float alpha = timeStep/(velLowpassTimeConstant + timeStep); //https://en.wikipedia.org/wiki/Low-pass_filter#Simple_infinite_impulse_response_filter

	//find xyzRube
	float lastLineLength[] = {lineLength[0],lineLength[1],lineLength[2]};
	lineLength[0] = lengthPerEncStep * encA.read();
	lineLength[1] = lengthPerEncStep * encB.read();
	lineLength[2] = lengthPerEncStep * encC.read();

	for(int i = 0; i < 3 ; ++i){ // for each line
		lineVelRube[i] = (1-alpha)*lineVelRube[i] + alpha*(lineLength[i] - lastLineLength[i])/timeStep;
	}

	float lastXyzRube[] = {xyzRube[0],xyzRube[1],xyzRube[2]};
	line2xyzRube(lineLength,  xyzRube);

	for( int j = 0; j < 3 ; ++j){//for xyz
		xyzVelRube[j] = (1-alpha)*xyzVelRube[j] + alpha*(xyzRube[j] - lastXyzRube[j])/timeStep;
	}

	//------ generate instantaneous position target ---------
	//TODO parse waypoints

	float instantaneousXyz2nextWP[3];
	for( int j = 0; j < 3 ; ++j){//for xyz
		instantaneousXyz2nextWP[j] = waypointXYZ[j][0] - instantaneousXyzTarget[j];
	}
	float distToNextWP = norm(instantaneousXyz2nextWP);

	//time required to accelerate to :
	float WPtransitionVel = 0;//average vel between current and next wp

	float timeToAccelerate;
	float distanceToAccelerate;
	if (speedTarget > WPtransitionVel){//need to brake before wp
		timeToAccelerate = (speedTarget - WPtransitionVel)/maxAcc;

		//distance required to accelerate to WPtransitionVel:
		distanceToAccelerate = speedTarget*timeToAccelerate - maxAcc*timeToAccelerate*timeToAccelerate/2;//TODO check signs
	}else{//need to accelerate before wp
		timeToAccelerate = (WPtransitionVel - speedTarget)/maxAcc;

		//distance required to accelerate to WPtransitionVel:
		distanceToAccelerate = speedTarget*timeToAccelerate + maxAcc*timeToAccelerate*timeToAccelerate/2;//TODO check signs
	}

	//integrate speedTarget
	if (distanceToAccelerate < (distToNextWP - timeStep*speedTarget)){// not close to next waypoint
		speedTarget+= timeStep*maxAcc;
		if(speedTarget > waypointSpeed[1]) speedTarget = waypointSpeed[1];
	}else{ // close to next waypoint
		if(speedTarget > WPtransitionVel){
			speedTarget-=timeStep*maxAcc;
		}else{
			speedTarget+=timeStep*maxAcc;
		}
	}

	if (distToNextWP > timeStep*speedTarget){
		for( int j = 0; j < 3 ; ++j){//for xyz
			//integrate instantaneousXyzTarget
			instantaneousXyzTarget[j] = instantaneousXyzTarget[j] + timeStep*speedTarget*instantaneousXyz2nextWP[j]/distToNextWP;
		}
	}else{
		for( int j = 0; j < 3 ; ++j){//for xyz
			instantaneousXyzTarget[j] = waypointXYZ[j][0];
		}
		//TODO increment active WP index
	}

	//------ position controller ---------

	//find posError
	float xyzError[3];
	vectorSubtract(instantaneousXyzTarget,xyzRube, xyzError);

	//find velTarget
	controllerX.update(xyzError[0], timeStep);
	controllerY.update(xyzError[1], timeStep);
	controllerZ.update(xyzError[2], timeStep);

	xyzVelTarget[0] = controllerX.output;
	xyzVelTarget[1] = controllerY.output;
	xyzVelTarget[2] = controllerZ.output;

	//cascade PID with outer = position control, inner = velocity control
	//------ velocity controller ---------
	float xyzForce[3];
	float lineForce[3];
	for( int j = 0; j < 3 ; ++j){//for xyz
		float xyzVelError= xyzVelTarget[j]-xyzVelRube[j];

		// P controller for now
		float xyzAccTarget = velControllerP*xyzVelError;

		//------   AccTarget to motorOutputs ---------
		xyzForce[j] = mass*xyzAccTarget;
	}
	xyzForce[2] = xyzForce[2] + gravitAcc;

	//find equivalent lineForce
	xyzForce2lineForce(xyzRube, xyzForce, lineForce);

	for(int i = 0; i < 3 ; ++i){ // for each line
		//TODO spoolRadius a function of lineLength
		//TODO frictionForce opposite direction of travel

		if (lineForce[i] < 0){
			lineForce[i] = 0;
		}

		float motorTorque;
		//lineForce = motorTorque*spoolRadius - frictionForce
		if (lineVelRube[i] > 0){
			motorTorque= (lineForce[i] + frictionForce)/spoolRadius[i];
		}else{
			motorTorque= (lineForce[i] - frictionForce)/spoolRadius[i];
		}

		// motor equations:
		// rpm = 60*lineVel/(spoolRadius*2*pi)
		// Utot = I*R + motorKv*rpm
		// motorTorque = I*motorKt
		//-> I = motorTorque/motorKt;
		float I = motorTorque/motorKt;
		float rpm = 60*lineVelRube[i]/(spoolRadius[i]*2*M_PI);
		float Utot = I*motorR + motorKv*rpm;
		motorPwm[i] = Utot/batteryVoltage;
	}

	setMotorPwms(motorPwm);
	parseCommands();

	lastLoopStart = loopStart;
}

//void  setAllPIDs(){
//	controllerA.SetTuningsUp(Pu,Iu,Du);
//	controllerB.SetTuningsUp(Pu,Iu,Du);
//	controllerC.SetTuningsUp(Pu,Iu,Du);
//
//	controllerA.SetTuningsDown(Pd,Id,Dd);
//	controllerB.SetTuningsDown(Pd,Id,Dd);
//	controllerC.SetTuningsDown(Pd,Id,Dd);
//
//	controllerA.SetOutputLimits(55,-5);
//	controllerB.SetOutputLimits(55,-5);
//	controllerC.SetOutputLimits(55,-5);
//	controllerA.SetSampleTime(10);
//	controllerB.SetSampleTime(10);
//	controllerC.SetSampleTime(10);
//}
