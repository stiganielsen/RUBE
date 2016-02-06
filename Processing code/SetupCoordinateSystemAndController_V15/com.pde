void send32Int(long integer){
  //myPortOut.clear();
  char temp;
  for(int i=0;i<4;i++){
    temp = (char)( (integer >> (24-i*8)) & 0xFF);
    myPortOut.write(temp);
  }
}
  
void sendXYZ(){
myPortOut.write('c');
send32Int(int(pointR.x));
send32Int(int(pointR.y));
send32Int(int(pointR.z));
println("Sending XYZ");
}

void sendPID(){
myPortOut.write('u');
send32Int(int(Pu*1000));
send32Int(int(Iu*1000));
send32Int(int(Du*1000));
myPortOut.write('d');
send32Int(int(Pd*1000));
send32Int(int(Id*1000));
send32Int(int(Dd*1000));


}

void SendCalibration(){
myPortOut.write('k');
send32Int(int(heightMount));
send32Int(int(distOP));
send32Int(int(distPQ));
send32Int(int(distOQ));
send32Int(int(distOR));
send32Int(int(distPR));
send32Int(int(distQR));
println("Sending Calibration data...");


}
