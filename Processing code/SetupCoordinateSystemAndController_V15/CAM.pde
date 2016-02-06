void saveSettings(){
String settings ="";
settings= Pu+","+Iu+","+Du+","+Pd+","+Id+","+Dd+","+motorsActive+","+pointH.x+","+pointH.y+","+pointH.z;
saveStrings("Settings.txt",settings.split(","));
println(settings);
println("Camera settings saved");

}

void graphSystem() {

posX++;
stroke(255,0,0);
strokeWeight(1);
line(0,height*.7,900,height*.7);
stroke(0,255,0);
strokeWeight(2);
point(posX,400);
//erase just in front of new draw
fill(255);
noStroke();
rect(posX,height*.2,4,height);

stroke(255,0,0);
if(posX%3==1)line(posX,(height*.7)+incoming [3]-incoming [6],posX,(height*.7)+incoming [3]-incoming [6]-incoming[9]/8);
stroke(0,255,0);
if(posX%3==2)line(posX,(height*.7)+incoming [4]-incoming [7],posX,(height*.7)+incoming [4]-incoming [7]-incoming[10]/8);
stroke(0,0,255);
if(posX%3==0)line(posX,(height*.7)+incoming [5]-incoming [8],posX,(height*.7)+incoming [5]-incoming [8]-incoming[11]/8);


//Tern
  strokeWeight(.2);
  stroke(210,210,210);
for(int i=10;i<900;i+=20){
 line(i,0,i,600);
 line(0,i,900,i);
 }
//move to start and overwrite
 
if(posX>width){ 
  posX=0;
 background(255);
}



}
/*
void draws(){
  strokeWeight(5);
  stroke(200, 0, 0);
  point(pointO.x, pointO.y, pointO.z);
  stroke(0, 200, 0);
  point(pointP.x, pointP.y, pointP.z);
  stroke(0, 0, 200);
  point(pointQ.x, pointQ.y, pointQ.z);
 
  
stroke(255,255,0);
 strokeWeight(20);
  point(pointH.x, pointH.y, pointH.z);
  stroke(0, 200, 0);
  strokeWeight(15); 
  point(pointR.x, pointR.y, pointR.z);
  
  stroke(200, 200, 0); //yellow Lines
  strokeWeight(2);
  line(pointO.x, pointO.y, pointO.z, pointR.x, pointR.y, pointR.z);
  line(pointP.x, pointP.y, pointP.z, pointR.x, pointR.y, pointR.z);
  line(pointQ.x, pointQ.y, pointQ.z, pointR.x, pointR.y, pointR.z);
  stroke(200,100,100);line(   pointR.x, pointR.y, pointR.z, 0, pointR.y, pointR.z);
  stroke(100,200,100);line(   pointR.x, pointR.y, pointR.z, pointR.x, 0, pointR.z);
  stroke(100,100,200);line(   pointR.x, pointR.y, pointR.z, pointR.x, pointR.y, 0);
}*/
