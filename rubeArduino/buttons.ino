#include "Arduino.h"
int butA=22;
int butB=21;
int butC=20;


void initButtons(){
	pinMode(butA,INPUT);
	pinMode(butB,INPUT);
	pinMode(butC,INPUT);
}

boolean isButtonAPressed(void){
	return digitalRead(butA)==HIGH;
}

boolean isButtonBPressed(void){
	return digitalRead(butB)==HIGH;
}

boolean isButtonCPressed(void){
	return digitalRead(butC)==HIGH;
}
