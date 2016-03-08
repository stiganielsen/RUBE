import java.awt.Frame;
import java.awt.BorderLayout;
import controlP5.*;
private ControlP5 cp5;
ControlFrame cf;
import processing.serial.*;
Serial myPortIn;  // Create object from Serial class
Serial myPortOut;
/* RUBE line robot hanging in three points with lines from them
 hang from points O,P,Q the lift is in point R (rube;) (on the ground plane: z=0)
*/
PFont metaBold;

 //HEIGHT(s)

float heightMount=3520;//16000;
//DISTANCES BETWEEN TOP PTS
float distOP= 7520;//ljusg15570;//crf628;//origo -> første  (o) O------P  (x)
float distPQ= 6782;//ljusg15980;//crf600;//første til anden      \  R /
float distOQ= 6000;//ljusg25640;//crf437;//anden til origo        \  /
//                                                           (y)   Q
//DISTANCES TO RUBE
float distOR =1253;//20750;
float distPR =7154;//16450; //1444 er højden fra gulv sjuss
float distQR =5392;//21520;

float lineO=distOR;
float lineP=distPR;
float lineQ=distQR;

float Pu =2;
float Iu =1;
float Du =0.5;

float Pd =0.2;
float Id =0.1;
float Dd =0.5;

float gox=0; //<>//
float goy=0;
float goz=0;
long inRx=0;
long inRy=0;
long inRz=0;
long inRo=0;
long inRp=0;
long inRq=0;

PVector pointO, pointP, pointQ, pointR, pointH;

float jogK=.8;
Boolean newData=false;
 int serialRxBufferMaxSize = 20;
 int posX=0;
float[] incoming = new float [15];
int stop=0;
int motorsActive=0;
///////////////////////////////////////////////////////////////////
void setup() {
  metaBold = loadFont("Monospaced.bold-48.vlw");
  textFont(metaBold);
  textSize(12);

  surface.setResizable(true);
  surface.setSize(1000, 800);
  frameRate(30);
  cp5 = new ControlP5(this);
  cf = new ControlFrame("extra", this, 400, 800);
  cf.GUIsetup();

  myPortIn = new Serial(this, "COM8", 9600);
  myPortOut= myPortIn;
  //myPortOut =new Serial(this, "COM8", 9600);
  pointO = new PVector(0, 0, heightMount);
  pointP = new PVector(distOP,0, heightMount); 
  //finding the XY of point Q
  float r1=distOQ;
  float r2=distPQ;
  float d=distOP;
  float qX=((d*d)-(r2*r2)+(r1*r1))/(2*d) ; 
  float qY=(1/d * sqrt((-d+r2-r1)*(-d-r2+r1)*(-d+r2+r1)*(d+r2+r1)))/2; 
println("pointQ.x"+qX+"  pointQ.y"+qY);

  pointQ = new PVector(qX, qY, heightMount); //to be calculated
  //now finding the pointR based on the three other points OPQ and the distances from these to pointR
  r1=distOR;
  r2=distPR;
  float r3=distQR;
  
  
  d=distOP;
  float i=qX; //pointQ.x
  float j=qY; //pointQ.y

  float xc=( ( (r1*r1) - (r2*r2) + (d*d) ) /  (2*d) );
  float yc= ( ( (r1*r1) - (r3*r3) + (i*i) + (j*j) ) / (2*j) ) - ( (i/j)*xc ); // ((r1*r1)-(r3*r3)-(xc*xc)+( (xc-i)*(xc-i) )+(j*j) / (2*j));
  float zc= sqrt ( (r1*r1) - (xc*xc) - (yc*yc) )  ;
  pointR = new PVector(xc, yc, heightMount-zc); //now zc should be zero???
  pointH = new PVector(xc, yc, heightMount-zc); //this is overwritten

  //moving the points up to their height
  
String settingLine[] = loadStrings("Settings.txt");
//settings= pU+","+iU+","+dU+","+pD+","+iD+","+dD;
//String settingLines[] =split(settingLine[0], ',');
Pu= float(settingLine[0]);
Iu= float(settingLine[1]);
Du= float(settingLine[2]);
Pd= float(settingLine[3]);
Id= float(settingLine[4]);
Dd= float(settingLine[5]);
motorsActive= int(settingLine[6]);
pointH.x =float(settingLine[7]);
pointH.y =float(settingLine[8]);
pointH.z =float(settingLine[9]);
//String[] setCam= split(settingLine[0], ',');

  println("the calculated X point for rube is: X: "+pointR.x);
  println("the calculated Y point for rube is: Y: "+pointR.y);
  println("the calculated Z point for rube is: Z: "+pointR.z);
  //println("RUbe error; thinks it hovers "+(pointR.z-heightO));
//myPort.clear();
 
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
void draw() {
 
  graphSystem();
  texts();
//  draws();
}
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//send these new coordinates to the RUBE via serial / and later Bluetooth.


void serialEvent (Serial myPortIn) {
  // get the ASCII string:
  String[] pieces=new String [22];
  String inString = myPortIn.readStringUntil('\t');  //read untill (\n) means newline
  pieces =  split(inString,',');
  
 if (inString != null) {
  for(int i=0;i<15;i++){
   incoming [i]= float(pieces[i]);
  }
  
    
 }inString="";//myPort.clear();
}
    