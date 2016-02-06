void texts(){
fill(240);
noStroke();
rect(0,0,width,height*.2);  
int textYspace=20;
fill(0,55,230);
//text("x: " + round(pointR.x) + " y: " + round(pointR.y) + " z: " + round(pointR.z), 0,textYspace); 
textYspace+=20;
fill(0,55,230);
text("inRx: " + incoming [0] + " inRy: " + incoming [1]  + " inRz: " + incoming [2]  , 0,textYspace); 
textYspace+=20;
fill(0,155,230);
text("motor O " + incoming [9] + " motor P " + incoming [10]  + " motor Q " + incoming [11]  , 0,textYspace); 
textYspace+=20;
fill(0,155,230);
text("pos O " + incoming [6] + " pos P " + incoming [7]  + " pos Q " + incoming [8]  , 0,textYspace); 
textYspace+=5;
if(incoming[12]<2) fill(0,255,0);
else fill(255,0,0);
rect(50,textYspace,80,20);
if(incoming[13]<2) fill(0,255,0);
else fill(255,0,0);
rect(150,textYspace,80,20);
if(incoming[14]<2) fill(0,255,0);
else fill(255,0,0);
rect(250,textYspace,80,20);

textYspace+=20;
fill(0,155,230);
text("       Tension O     Tension P     Tension Q", 0,textYspace-5);



}
