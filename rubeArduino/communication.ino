#include "Arduino.h"
#define BTSerial Serial1
const int btPacketSize = 1+(4*3);
String stringToSend="";

extern Controller controllerA;
extern Controller controllerB;
extern Controller controllerC;

extern int lineO;
extern int lineP;
extern int lineQ;

extern float heightO;
extern float distOP;
extern float distPQ;
extern float distOQ;
extern float distOR;
extern float distPR;
extern float distQR;

extern long posNow[];
extern float xyzRube[];
int stopCmd=0;

void initCommunication(void){
	Serial.begin(9600);
	BTSerial.begin(9600);
}

void sendBuffer(void){
	stringToSend+= posNow[0];
	stringToSend+= ",";
	stringToSend+= posNow[1];
	stringToSend+= ",";
	stringToSend+= posNow[2];
	stringToSend+= ",";
	stringToSend+= lineO;//3
	stringToSend+= ",";
	stringToSend+= lineP;
	stringToSend+= ",";
	stringToSend+= lineQ;
	stringToSend+= ",";
	stringToSend+= int(posNow[0]/2);
	stringToSend+= ",";
	stringToSend+= int(posNow[1]/2);
	stringToSend+= ",";
	stringToSend+= int(posNow[2]/2);
	stringToSend+= ",";
	stringToSend+= controllerA.Output;//[9]
	stringToSend+= ",";
	stringToSend+= controllerB.Output;
	stringToSend+= ",";
	stringToSend+= controllerC.Output;
	stringToSend+= ",";
	if(isButtonAPressed()) stringToSend+=2 ;
	else stringToSend+=1;
	stringToSend+= ",";
	if(isButtonBPressed()) stringToSend+=2 ;
	else stringToSend+=1 ;
	stringToSend+= ",";
	if(isButtonCPressed()) stringToSend+=2 ;
	else stringToSend+=1;//14 */
	//stringToSend+= ",";stringToSend+= "666";
	//stringToSend+= ",";stringToSend+= "666";stringToSend+= ",";stringToSend+= "666";stringToSend+= ",";stringToSend+= "666";
	stringToSend+='\t';
	BTSerial.print(stringToSend);
	BTSerial.println();
	stringToSend="";
}

long parseInt32(){
  char d1 = (char)BTSerial.read();
  char d2 = (char)BTSerial.read();
  char d3 = (char)BTSerial.read();
  char d4 = (char)BTSerial.read();

  return ((d1<<24) +(d2<<16) + (d3<<8) + d4);
}

// TODO use: https://github.com/duff2013/UartEvent
void serialEvent(){
	//Serial.println("Bluetooth signal ");
	while(BTSerial.available()>=btPacketSize) { //have we received a full data packet yet?
		tone(24,1000,10);
		char dataIdentifyer = (char)BTSerial.read();
		switch (dataIdentifyer) {
		case 'c': //coordinates
			xyzRube[0] = int(parseInt32());
			xyzRube[1] = int(parseInt32());
			xyzRube[2] = int(parseInt32());
			break;
		case 'u': //pid-parameters up
			Pu = float(parseInt32());Pu=Pu/1000;
			Iu = float(parseInt32());Iu=Iu/1000;
			Du = float(parseInt32());Du=Du/1000;
//							setAllPIDs();
			break;
		case 'd': //pid-parameters down
//			Pd = float(parseInt32());Pd=Pd/1000;
//			Id = float(parseInt32());Pd=Pd/1000;
//			Dd = float(parseInt32());Pd=Pd/1000;
//							setAllPIDs();
			break;
		case 's':
			stopCmd=float(parseInt32());
			break;
		case 'k':
			heightO= float(parseInt32());//16000; //currently the rube must be mounted so that the points are in the same height.
			distOP= float(parseInt32());
			distPQ= float(parseInt32());
			distOQ= float(parseInt32());
			distOR =float(parseInt32());
			distPR =float(parseInt32());
			distQR =float(parseInt32());
		}
		//BTSerial.clear();
		//  Serial.print("received serial x: ");
		// Serial.println(Rx);
	} //END WHILE

}
