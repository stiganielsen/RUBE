long parseInt32(){
  char d1 = (char)BTSerial.read(); 
  char d2 = (char)BTSerial.read(); 
  char d3 = (char)BTSerial.read(); 
  char d4 = (char)BTSerial.read(); 
   
  return ((d1<<24) +(d2<<16) + (d3<<8) + d4);
}
  
  
void  setAllPIDs(){
pidA.SetTuningsUp(Pu,Iu,Du);
pidB.SetTuningsUp(Pu,Iu,Du);
pidC.SetTuningsUp(Pu,Iu,Du);

pidA.SetTuningsDown(Pd,Id,Dd);
pidA.SetTuningsDown(Pd,Id,Dd);
pidA.SetTuningsDown(Pd,Id,Dd);
}

