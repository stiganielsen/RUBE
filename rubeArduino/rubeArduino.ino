#include "Controller.h"
#include <Encoder.h>

extern float lineAttachment[3][3];
float lineLength[3];
float lineVelRube[3];
float xyzRube[3];
float xyzVelRube[3];

int state = 0;
int activeAction = 0;

uint32_t lastLoopStart;
float timeStep;
#define HWSERIAL Serial1

float velLowpassTimeConstant = 0.15;

//------ action parameters ---------
volatile uint32_t actionStopTime = 0;

//------ waypoint parameters ---------
float waypointXYZ[3][2]; //TODO list of waypoints
float waypointSpeed[2]= { 0.1, 0.1};
float speedTarget = 0.1; //m/s
float maxAcc = 0.1;

//------ position controller ---------
float xyzTarget[3];
float posControllerP = 1;
float posControllerI = 0;
float posControllerD = 0;

Controller controllerX = Controller(posControllerP, posControllerI, posControllerD);
Controller controllerY = Controller(posControllerP, posControllerI, posControllerD);
Controller controllerZ = Controller(posControllerP, posControllerI, posControllerD);

//------ velocity controller ---------
volatile float xyzVelTarget[3]={0, 0, 0};
float velControllerP = 10;
float velControllerI = 0;
float velControllerD = 0;

Controller controllerXvel = Controller(velControllerP, velControllerI, velControllerD);
Controller controllerYvel = Controller(velControllerP, velControllerI, velControllerD);
Controller controllerZvel = Controller(velControllerP, velControllerI, velControllerD);

// force calculations
float mass = 2; //kg
float gravitAcc = 9.82; //m/s^2
float spoolRadius[3]={ 0.05, 0.05, 0.05};
float frictionForce = 4;

// maxon DC motor 280817 properties: (very similar to 273758)
float motorKv = 85;//5; //rpm/Volt
float motorKt = 9.5/motorKv; //Nm/A
float motorR = 7; //Ohm
float batteryVoltage = 12; //Volt, max: 48V
float motorPwm[3]; // -1 to 1

/*
 * maxon DC motor 280817 properties: very similar to 273758
 * R = 7 ohm
 * kv = 85 rpm/V
 */

Encoder encA(28,29);
Encoder encB(27,30);
Encoder encC(31,26);
float lengthPerEncStep = 0.0005;

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

	//TODO calibrate OPQ from given rubeXyz, measured line-angles
	//TODO estimate payload weight
	//TODO multiple ways of calibrating?

	//	//DISTANCES TO RUBE
	//	float distOR =2.600;//20750;
	//	float distPR =2.870;//16450; //1444 er højden fra gulv sjuss
	//	float distQR =2.456;//21520;
	//TODO calibrate xyz with distance sensor
//
//	//assuming rube is centered and on the floor, find average x and y of lineAttachements
//	xyzRube[0] = (lineAttachment[0][0] + lineAttachment[1][0] + lineAttachment[2][0])/3;
//	xyzRube[1] = (lineAttachment[0][1] + lineAttachment[1][1] + lineAttachment[2][1])/3;
//	xyzRube[2] = 1;
//
//	xyzRube2line(xyzRube,  lineLength);
	//	printStatus();

	//rube parking spot in make-lab
	lineLength[0] = 4.456;
	lineLength[1] = 6.171;
	lineLength[2] = 5.009;
	line2xyzRube(lineLength,xyzRube);


	float temp = 1/lengthPerEncStep;

	encA.write((int32_t)(temp*lineLength[0]));
	encB.write((int32_t)(temp*lineLength[1]));
	encC.write((int32_t)(temp*lineLength[2]));

	waypointXYZ[0][0] = xyzRube[0];
	waypointXYZ[1][0] = xyzRube[1];
	waypointXYZ[2][0] = xyzRube[2];

	waypointXYZ[0][1] = xyzRube[0];
	waypointXYZ[1][1] = xyzRube[1];
	waypointXYZ[2][1] = xyzRube[2] + 100;
}

void loop(){
	uint32_t loopStart = millis();
	timeStep = 0.001*(loopStart - lastLoopStart);
	//TODO moving average filter
	float alpha = timeStep/(velLowpassTimeConstant + timeStep); //https://en.wikipedia.org/wiki/Low-pass_filter#Simple_infinite_impulse_response_filter

	//update lineLength, lineVelRube, xyzRube, xyzVelRube
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

	/* number = action -> followingState
	 * 0 = do nothing
	 * 1 = tension lines -> 2
	 * 2 = hold position -> 5
	 * 3 = controlLineVel -> 2
	 * 4 = controlXyzVel -> 2
	 * 5 = controlXyzPos -> 2
	 * 6 = parseActions -> 2
	 * 7 = perform autonomous calibration -> 2
	 * 8 = anti gravity mode -> 2
	 */
	switch (state) {
	case 0:// do nothing
		stopMotors();
		break;
	case 1: // tension lines
		//		if (!(isButtonAPressed() || isButtonBPressed() || isButtonCPressed())) {
		//			HWSERIAL.println("done tensioning");
		//			state = 0;
		//		}else{
		HWSERIAL.println("tensioning");
		tensionLines();
		//		}
		break;
	case 2: // hold position, set xyzTarget to current
		xyzVelTarget[0]= 0;
		xyzVelTarget[1]= 0;
		xyzVelTarget[2]= 0;
		xyzVelControl();
		//		xyzControl();
		break;
	case 3: // controlLineVel
		//TODO find equivalent lineVels
		xyzVelControl();
		break;
	case 4: // controlXyzVel
		//		HWSERIAL.print("time left: ");HWSERIAL.println(actionStopTime - millis());
		if(millis()>actionStopTime){

			state = 0;
		}else{
			xyzVelControl();
		}
		break;
	case 5: // controlXyzPos
		//TODO set xyzTarget
		xyzControl();
		break;
	case 6: // parseActions

		/* 0 = none
		 * 1 = bufWPfine
		 * 2 = bufWPrough
		 * 3 = bufPause
		 * 4 = bufGripper
		 * 5 = bufFan
		 */
		switch (activeAction) {
		case 0: // none
			state = 2;
			break;
		case 1: // bufWPfine
			//TODO
			break;
		case 2: // bufWPrough
			//------ generate instantaneous position target ---------
			//TODO parse waypoints

			float instantaneousXyz2nextWP[3];
			for( int j = 0; j < 3 ; ++j){//for xyz
				instantaneousXyz2nextWP[j] = waypointXYZ[j][0] - xyzTarget[j];
			}
			float distToNextWP;
			distToNextWP = norm(instantaneousXyz2nextWP);

			//time required to accelerate to :
			float WPtransitionVel;//average vel between current and next wp
			WPtransitionVel = 0;

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
					//integrate xyzTarget
					xyzTarget[j] = xyzTarget[j] + timeStep*speedTarget*instantaneousXyz2nextWP[j]/distToNextWP;
				}
			}else{
				for( int j = 0; j < 3 ; ++j){//for xyz
					xyzTarget[j] = waypointXYZ[j][0];
				}
				//TODO increment active WP index
			}

			xyzControl();
			break;
		case 3: // bufPause
			xyzVelTarget[0]= 0;
			xyzVelTarget[1]= 0;
			xyzVelTarget[2]= 0;
			xyzVelControl();
			//			xyzControl(); //TODO set xyzTarget to current
			break;
		case 4: // bufGripper
			//TODO
			break;
		case 5: // bufFan
			//TODO
			break;
		}

		break;
		case 7: // perform autonomous calibration
			//TODO
			break;
		case 8: // anti gravity mode
//			if (!(isButtonAPressed() || isButtonBPressed() || isButtonCPressed())) {
//				tensionLines();
//			}else{
				//vel error is always 0 -> only compensate for weight
				xyzVelTarget[0] = xyzVelRube[0];
				xyzVelTarget[1] = xyzVelRube[1];
				xyzVelTarget[2] = xyzVelRube[2];
				xyzVelControl();
//			}
			break;
	}

	parseCommands();
	lastLoopStart = loopStart;
	delay(1);
}

void tensionLines(){
	if(isButtonAPressed()){
		setMotorPwm(0, 0.7);
	}else{
		setMotorPwm(0, -0.4);
	}

	if(isButtonBPressed()){
		setMotorPwm(1, 0.7);
	}else{
		setMotorPwm(1, -0.4);
	}

	if(isButtonCPressed()){
		setMotorPwm(2, 0.7);
	}else{
		setMotorPwm(2, -0.4);
	}
}

/*
 * assuming xyzTarget is updated
 * writes directly to motors
 */

void xyzControl(){

	HWSERIAL.print("xyzTarget X: "); HWSERIAL.print(xyzTarget[0],3);
	HWSERIAL.print("          Y: "); HWSERIAL.print(xyzTarget[1],3);
	HWSERIAL.print("          Z: "); HWSERIAL.println(xyzTarget[2],3);
	//find posError
	float xyzError[3];
	vectorSubtract(xyzTarget,xyzRube, xyzError);

	//find velTarget
	controllerX.update(xyzError[0], timeStep);
	controllerY.update(xyzError[1], timeStep);
	controllerZ.update(xyzError[2], timeStep);

	xyzVelTarget[0] = controllerX.output;
	xyzVelTarget[1] = controllerY.output;
	xyzVelTarget[2] = controllerZ.output;

	xyzVelControl();

}
/*
 * assuming xyzVelTarget is updated
 * writes directly to motors
 */
void xyzVelControl(){
	HWSERIAL.print("velTarget X: "); HWSERIAL.print(xyzVelTarget[0],3);
	HWSERIAL.print("          Y: "); HWSERIAL.print(xyzVelTarget[1],3);
	HWSERIAL.print("          Z: "); HWSERIAL.println(xyzVelTarget[2],3);
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

	xyzForce[2] = xyzForce[2] + mass*gravitAcc;
	HWSERIAL.print("forceTarget: "); HWSERIAL.print(xyzForce[0]);
	HWSERIAL.print(", ");HWSERIAL.print(xyzForce[1]);
	HWSERIAL.print(", ");HWSERIAL.println(xyzForce[2]);


	//find equivalent lineForce
	xyzForce2lineForce(xyzRube, xyzForce, lineForce);
	HWSERIAL.print("line Target: "); HWSERIAL.print(lineForce[0]);
	HWSERIAL.print(", "); HWSERIAL.print(lineForce[1]);
	HWSERIAL.print(", "); HWSERIAL.println(lineForce[2]);

	for(int i = 0; i < 3 ; ++i){ // for each line
		//TODO spoolRadius a function of lineLength
		//TODO frictionForce opposite direction of travel

		if (lineForce[i] < 0){
			lineForce[i] = 0;
		}

		float motorTorque;
		//lineForce = motorTorque*spoolRadius - frictionForce
		if (lineVelRube[i] > 0){
			motorTorque= (lineForce[i] + frictionForce)*spoolRadius[i];
		}else{
			motorTorque= (lineForce[i] - frictionForce)*spoolRadius[i];
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
		HWSERIAL.print("I: "); HWSERIAL.print(I);HWSERIAL.print(", ");
		HWSERIAL.print("rpm: "); HWSERIAL.print(rpm);HWSERIAL.print(", ");
		HWSERIAL.print("Utot: "); HWSERIAL.println(Utot);


	}
	HWSERIAL.print("motorPwm: "); HWSERIAL.print(motorPwm[0]);
	HWSERIAL.print(", "); HWSERIAL.print(motorPwm[1]);
	HWSERIAL.print(", "); HWSERIAL.println(motorPwm[2]);

	//	setMotorPwms(motorPwm);
	setMotorPwm(0, motorPwm[0]);
	setMotorPwm(1, motorPwm[1]);
	setMotorPwm(2, motorPwm[2]);
}
