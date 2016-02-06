void motor(int abc,int spd){
  switch(abc) {
  case 0:
    if (spd==0) { digitalWrite (pinDirA1,HIGH);digitalWrite (pinDirA2,HIGH);analogWrite(pinSpdA, abs(spd));}//analogWrite(pinSpdA, 200);}
    else if(spd>0){digitalWrite(pinDirA1,HIGH); digitalWrite (pinDirA2,LOW);analogWrite(pinSpdA, abs(spd));}
    else if(spd<0){digitalWrite(pinDirA1,LOW); digitalWrite (pinDirA2,HIGH);analogWrite(pinSpdA, abs(spd));}
    break;
 case 1:
    if (spd==0) { digitalWrite (pinDirB1,HIGH);digitalWrite (pinDirB2,HIGH);analogWrite(pinSpdB, abs(spd));}
    else if(spd>0){digitalWrite(pinDirB1,HIGH); digitalWrite (pinDirB2,LOW);analogWrite(pinSpdB, abs(spd));}
    else if(spd<0){digitalWrite(pinDirB1,LOW); digitalWrite (pinDirB2,HIGH);analogWrite(pinSpdB, abs(spd));}
    break;
 case 2:
    if (spd==0) { digitalWrite (pinDirC1,HIGH);digitalWrite (pinDirC2,HIGH);analogWrite(pinSpdC, abs(spd));}
    else if(spd>0){digitalWrite(pinDirC1,HIGH); digitalWrite (pinDirC2,LOW);analogWrite(pinSpdC, abs(spd));}
    else if(spd<0){digitalWrite(pinDirC1,LOW); digitalWrite (pinDirC2,HIGH);analogWrite(pinSpdC, abs(spd));}
    break;
  default:
    break;
  }
}

