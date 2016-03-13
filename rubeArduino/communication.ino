#include "Arduino.h"

#define BTSerial Serial1
String stringToSend="";

// set this to the hardware serial port you wish to use
#define HWSERIAL Serial1
#define IN_BUFFER_SIZE 40
volatile char inBuffer[IN_BUFFER_SIZE];
volatile boolean inBufferIsFilled=false;
volatile int inBufferedChars = 0;
volatile boolean lineIsComment = false;

void initCommunication(void){
	HWSERIAL.begin(9600);
	HWSERIAL.println("Welcome to RUBE! To get help, write 'help'");
}

void parseCommands(){
	while (inBufferIsFilled){
		char helpCharArray[] = 			"help";
		char statusCharArray[] = 		"status";
		char emergencyStopCharArray[] = "e";
		char stopCharArray[] = 			"stop";
		char continueCharArray[] = 		"continue";
		char resetWPsCharArray[] = 		"resetWPs";
		char velCartesianCharArray[] = 	"velCartesian";
		char velLinesCharArray[] = 		"velLines";

		char calLengthsCharArray[] = 	"calLengths";
		char calAccCharArray[] = 		"calAcc";
		char calPosPIDCharArray[] = 	"calPosPID";
		char calVelPIDCharArray[] = 	"calVelPID";
		char calFrictionCharArray[] = 	"calFriction";

		char bufWPfineCharArray[] = 	"bufWPfine";
		char bufWProughCharArray[] = 	"bufWPrough";
		char bufPauseCharArray[] = 		"bufPause";
		char bufGripperCharArray[] = 	"bufGripper";
		char bufFanCharArray[] = 		"bufFan";

		//copy content of inBuffer to another location
		int i = 0; //pointing to "active" character
		char tempBuffer[IN_BUFFER_SIZE];
		while(inBuffer[i] != 0){
			tempBuffer[i] = inBuffer[i];
			i++;
		}
		tempBuffer[i] = 0; // null-terminate string

		i = 0;

//		printChars(tempBuffer+i);
		if (isCharArrayAinBeginningOfB(helpCharArray, tempBuffer, &i)){
			HWSERIAL.println("here is some help :)");
			printChars(tempBuffer+i);
//			//TODO help
//			HWSERIAL.println(helpCharArray[0]);
//			HWSERIAL.println(inBuffer[0]);
//		    float tempFloat = parseWrittenFloat((char*)tempBuffer,(char**) &tempBuffer);
//		    HWSERIAL.println(tempFloat);
		}else if (isCharArrayAinBeginningOfB(statusCharArray, tempBuffer, &i)){
//			HWSERIAL.println("2");
			printStatus();
//			TODO status
		}else if (isCharArrayAinBeginningOfB(emergencyStopCharArray,tempBuffer, &i)){
			HWSERIAL.println("3");
			//TODO emergency stop
		}else if (isCharArrayAinBeginningOfB(stopCharArray,tempBuffer, &i)){
			HWSERIAL.println("4");
			//TODO stop
		}else if (isCharArrayAinBeginningOfB(continueCharArray,tempBuffer, &i)){
			HWSERIAL.println("5");
			//TODO continue
		}else if (isCharArrayAinBeginningOfB(resetWPsCharArray,tempBuffer, &i)){
			HWSERIAL.println("6");
			//TODO reset wps
		}else if (isCharArrayAinBeginningOfB(velCartesianCharArray,tempBuffer, &i)){
			HWSERIAL.println("7");
			//TODO velCartesian
		}else if (isCharArrayAinBeginningOfB(velLinesCharArray,tempBuffer, &i)){
			HWSERIAL.println("8");
			//TODO velLines
		}else if (isCharArrayAinBeginningOfB(calLengthsCharArray,tempBuffer, &i)){
			HWSERIAL.println("9");
			//TODO calLengths
		}else if (isCharArrayAinBeginningOfB(calAccCharArray,tempBuffer, &i)){
			HWSERIAL.println("10");
			//TODO calAcc
		}else if (isCharArrayAinBeginningOfB(calPosPIDCharArray,tempBuffer, &i)){
			HWSERIAL.println("11");
			//TODO calPosPID
		}else if (isCharArrayAinBeginningOfB(calVelPIDCharArray,tempBuffer, &i)){
			HWSERIAL.println("12");
			//TODO calVelPID
		}else if (isCharArrayAinBeginningOfB(calFrictionCharArray,tempBuffer, &i)){
			HWSERIAL.println("13");
			//TODO cal friction
		}else if (isCharArrayAinBeginningOfB(bufWPfineCharArray,tempBuffer, &i)){
			HWSERIAL.println("14");
			//TODO buf WP fine
		}else if (isCharArrayAinBeginningOfB(bufWProughCharArray,tempBuffer, &i)){
			HWSERIAL.println("15");
			//TODO buf WP rough
		}else if (isCharArrayAinBeginningOfB(bufPauseCharArray,tempBuffer, &i)){
			HWSERIAL.println("16");
			//TODO buf pause
		}else if (isCharArrayAinBeginningOfB(bufGripperCharArray,tempBuffer, &i)){
			HWSERIAL.println("17");
			//TODO buf gripper
		}else if (isCharArrayAinBeginningOfB(bufFanCharArray,tempBuffer, &i)){
			HWSERIAL.println("18");
			//TODO buf fan
		}else{
			HWSERIAL.print("command not recognized: ");
			printChars(tempBuffer+i);
		}



		inBufferIsFilled = false;
		inBufferedChars = 0;
	}
}

void printStatus(void){
	HWSERIAL.print("rube X:"); HWSERIAL.print(xyzRube[0]);
	HWSERIAL.print(" Y:"); HWSERIAL.print(xyzRube[1]);
	HWSERIAL.print(" Z:"); HWSERIAL.println(xyzRube[2]);

	HWSERIAL.print("line A:"); HWSERIAL.print(lineLength[0]);
	HWSERIAL.print(" B:"); HWSERIAL.print(lineLength[1]);
	HWSERIAL.print(" C:"); HWSERIAL.println(lineLength[2]);
}

/*
 * examples:
 * 45454
 * 3.4
 * 3,4
 * -3
 * -34389.334
 */
float parseWrittenFloat(char* buf, char** remainingBuf){
	boolean done = false;
	boolean positive = true;
	int decimalPlace = -1; //if -1, not found yet
	int i = 0;
	if (buf[i]== '-'){ // sign
		positive = false;
		++i;
	}
	float result = 0;
	while(!done){
		HWSERIAL.print(buf[i]);
		//'0' -> 48
		//'9' -> 57
		if ((buf[i] >= 48) && (buf[i] <= 57)){//integer
			int temp = buf[i] - 48;
			result = result*10 + (float)temp;
		}else if((buf[i]== '.') || buf[i]== ','){//decimals
			//TODO only allow one decimal in a number
			decimalPlace = i;

		}else{//end of number
			done=true;
			*remainingBuf = buf + i;
		}
		i++;
	}
	if (decimalPlace != -1){
		for(int j = 0; j < (i - 1 - decimalPlace); j++){
			result= 0.1*result;
		}
	}
	if (positive)return result;
	else return -result;

}

/*
 *  length of a must be equal or smaller than b.
 *  both strings must be null-terminated
 *  returning c =  array b with a removed
 */
boolean isCharArrayAinBeginningOfB(char* a, char* b, int* iPtr){
	int j = 0;
	while(true){
		if (a[j] == 0){//end of string
			*iPtr += j;
			return true;
		}else if(b[j+*iPtr] == 0){
			return false;
		}else if(a[j] != b[j+*iPtr]){
			return false;
		}
		j++;
	}
}

/*
 *  string must be null-terminated
 */
void printChars(volatile char* buf){
	int i = 0;
	while (buf[i] != 0){
		HWSERIAL.print(buf[i]);
		i++;
	}
	HWSERIAL.print('\n');
}


/*
  SerialEvent1 occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent1() {
	char tempChar;
	while ((HWSERIAL.available() > 0) && !inBufferIsFilled) {
		tempChar = HWSERIAL.read();
		if ((tempChar == '\n') || (tempChar == '\r')){
			if ((inBufferedChars == 0) || lineIsComment){
				lineIsComment = false;
				inBufferedChars = 0;
			}else{
				inBuffer[inBufferedChars] = 0; //null-terminated string
				//TODO not allow 0=null in the bufferedChars
				inBufferedChars++;
				inBufferIsFilled = true;
			}
		}else if (!lineIsComment){
			if ((tempChar == ' ') || (tempChar == '\t')){
				//throw away
			}else if (tempChar == '%'){
				lineIsComment = true;
			}else{
				if(inBufferedChars < IN_BUFFER_SIZE ){
					//store in inBuffer
					inBuffer[inBufferedChars] = tempChar;
					inBufferedChars++;
				}else{
					//error, no space in buffer!
				}
			}
		}
	}
}



//void sendBuffer(void){
//	stringToSend+= posNow[0];
//	stringToSend+= ",";
//	stringToSend+= posNow[1];
//	stringToSend+= ",";
//	stringToSend+= posNow[2];
//	stringToSend+= ",";
//	stringToSend+= lineO;//3
//	stringToSend+= ",";
//	stringToSend+= lineP;
//	stringToSend+= ",";
//	stringToSend+= lineQ;
//	stringToSend+= ",";
//	stringToSend+= int(posNow[0]/2);
//	stringToSend+= ",";
//	stringToSend+= int(posNow[1]/2);
//	stringToSend+= ",";
//	stringToSend+= int(posNow[2]/2);
//	stringToSend+= ",";
//	stringToSend+= controllerA.output;//[9]
//	stringToSend+= ",";
//	stringToSend+= controllerB.output;
//	stringToSend+= ",";
//	stringToSend+= controllerC.output;
//	stringToSend+= ",";
//	if(isButtonAPressed()) stringToSend+=2 ;
//	else stringToSend+=1;
//	stringToSend+= ",";
//	if(isButtonBPressed()) stringToSend+=2 ;
//	else stringToSend+=1 ;
//	stringToSend+= ",";
//	if(isButtonCPressed()) stringToSend+=2 ;
//	else stringToSend+=1;//14 */
//	//stringToSend+= ",";stringToSend+= "666";
//	//stringToSend+= ",";stringToSend+= "666";stringToSend+= ",";stringToSend+= "666";stringToSend+= ",";stringToSend+= "666";
//	stringToSend+='\t';
//	BTSerial.print(stringToSend);
//	BTSerial.println();
//	stringToSend="";
//}

long parseInt32(){
  char d1 = (char)BTSerial.read();
  char d2 = (char)BTSerial.read();
  char d3 = (char)BTSerial.read();
  char d4 = (char)BTSerial.read();

  return ((d1<<24) +(d2<<16) + (d3<<8) + d4);
}
