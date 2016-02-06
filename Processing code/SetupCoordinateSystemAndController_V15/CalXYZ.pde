void CalcXYZ()
{
  float r1=lineO;
  float r2=lineP;
  float r3=lineQ;
  float d=distOP;
  float i=pointQ.x;//qX; 
  float j=pointQ.y;//qY;

  float xc=( ( (r1*r1) - (r2*r2) + (d*d) ) /  (2*d) );
  float yc= ( ( (r1*r1) - (r3*r3) + (i*i) + (j*j) ) / (2*j) ) - ( (i/j)*xc ); // ((r1*r1)-(r3*r3)-(xc*xc)+( (xc-i)*(xc-i) )+(j*j) / (2*j));
  float zc= sqrt ( (r1*r1) - (xc*xc) - (yc*yc) )  ;
  pointR = new PVector(xc, yc, heightMount-zc); //now zc should be zero???
  print(" X: "+pointR.x);
  print(" Y: "+pointR.y);
  println(" Z: "+pointR.z);
}

void CalcLineOPQ(float x, float y, float z) {
  gox=x;
  goy=y;
  goz=z;
  float h=heightMount;
  float d= distOP;
  float i= pointQ.x;
  float j= pointQ.y;

  lineO =sqrt((gox*gox)+(goy*goy)+((goz-h)*(goz-h))); 
  lineP =sqrt(((gox-d)*(gox-d))+(goy*goy)+((goz-h)*(goz-h))) ;
  lineQ =sqrt(((gox-i)*(gox-i))+((goy-j)*(goy-j))+((goz-h)*(goz-h))) ;
  print(" lineO: "+lineO);
  print(" lineP: "+lineP);
  println(" lineQ: "+lineQ);
}
