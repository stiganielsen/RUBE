
ControlFrame addControlFrame(String theName, int theWidth, int theHeight) {
  Frame f = new Frame(theName);
  ControlFrame p = new ControlFrame(this, theWidth, theHeight);
  f.add(p);
  p.init();
  f.setTitle(theName);
  f.setSize(p.w, p.h);
  f.setLocation(5, 5);
  f.setResizable(false);
  f.setVisible(true);
  return p;
}


// the ControlFrame class extends PApplet, so we 
// are creating a new processing applet inside a
// new frame with a controlP5 object loaded
public class ControlFrame extends PApplet {
  int w, h;
  Textlabel explainA;
  Textlabel explainB;
  Textlabel explainC; 
  Textlabel explainD;
  Textlabel explainE;
  Textlabel explainF;
  Textlabel explainG;
  int abc = 100;
  public void setup() {
    size(w, h);
    frameRate(25);
    
    int space=0;
    cp5 = new ControlP5(this);
    //  cp5.addTextfield("heightO").setPosition(20,space).setAutoClear(false).setValue(12.3);
    //----------------------Line Jogging
   cp5.addBang("CalibrateRube").setPosition(145, space).setSize(61, 21).setLabel("RE-CALIBRATE RUBE ");

    if (lineO<500)  explainA = cp5.addTextlabel("labelI").setText("Bluetooth IS available").setPosition(10, 10).setColorValue(0xffffff00);
    else  explainA = cp5.addTextlabel("labelI").setText("Bluetooth NOT available").setPosition(10, 10).setColorValue(0xffffff00);
    space+=20;

    explainA = cp5.addTextlabel("labelA").setText("Jogging strings").setPosition(10, space).setColorValue(0xffffff00);
    space+=20;
    explainA = cp5.addTextlabel("labelB").setText("A").setPosition(40, space+5).setColorValue(0xffffff00);
    for (int i=-15; i<16; i+=5) {
      cp5.addBang("lineA"+i).setPosition(170+i*8, space).setSize(41, 21).setId(i).setLabel("    "+(i*i)*jogK);
    }
    space+=35;
    explainA = cp5.addTextlabel("labelC").setText("B").setPosition(40, space+5).setColorValue(0xffffff00);
    for (int i=-15; i<16; i+=5) {
      cp5.addBang("lineB"+i).setPosition(170+i*8, space).setSize(41, 21).setId(i).setLabel("    "+(i*i)*jogK);
    }
    space+=35;
    explainA = cp5.addTextlabel("labelD").setText("C").setPosition(40, space+5).setColorValue(0xffffff00);
    for (int i=-15; i<16; i+=5) {
      cp5.addBang("lineC"+i).setPosition(170+i*8, space).setSize(41, 21).setId(i).setLabel("    "+(i*i)*jogK);
    }
    space+=35;
    //    --------------------Coordinates XYZ
    explainA = cp5.addTextlabel("labelE").setText("Jogging Cartesian coordinates").setPosition(10, space).setColorValue(0xffffff00);
    space+=20;
    explainA = cp5.addTextlabel("labelF").setText("X").setPosition(40, space+5).setColorValue(0xffffff00);
    for (int i=-15; i<16; i+=5) {
      cp5.addBang("coorX"+i).setPosition(170+i*8, space).setSize(41, 21).setId(i).setLabel("    "+(i*i)*jogK);
    }
    space+=35;
    explainA = cp5.addTextlabel("labelG").setText("Y").setPosition(40, space+5).setColorValue(0xffffff00);
    for (int i=-15; i<16; i+=5) {
      cp5.addBang("coorY"+i).setPosition(170+i*8, space).setSize(41, 21).setId(i).setLabel("    "+(i*i)*jogK);
    }
    space+=35;
    explainA = cp5.addTextlabel("labelH").setText("Z").setPosition(40, space+5).setColorValue(0xffffff00);
    for (int i=-15; i<16; i+=5) {
      cp5.addBang("coorZ"+i).setPosition(170+i*8, space).setSize(41, 21).setId(i).setLabel("    "+(i*i)*jogK);
    }
    space+=45;  
    //----------------------HOME button
    cp5.addBang("home").setPosition(45, space).setSize(81, 21).setLabel(" GO TO HOME ");
    cp5.addBang("setHome").setPosition(145, space).setSize(21, 21).setLabel(" SET HOME ");

    space+=45;  
    //----------------------HOME button
    cp5.addToggle("motorsOn").setPosition(45, space).setSize(81, 81);//.setValue(false);//.setMode(ControlP5.SWITCH);

//    cp5.addBang("stop").setPosition(45, space).setSize(81, 81).setLabel(" STOP ");
    space+=105;  
    //-----------------------SLIDER  
    cp5.addSlider("moveSpeed").plugTo(parent, "moveSpeed").setRange(0, 100).setValue(50).setPosition(45, space).setSize(250, 21).setValue(50) ;
    space+=35; 
     cp5.addSlider("pU").plugTo(parent, "pU").setRange(0, 4).setValue(2).setPosition(45, space).setSize(250, 16) ;
    space+=35; 
     cp5.addSlider("iU").plugTo(parent, "iU").setRange(0, 3).setValue(1).setPosition(45, space).setSize(250, 16) ;
    space+=35; 
     cp5.addSlider("dU").plugTo(parent, "dU").setRange(0, 1).setValue(.5).setPosition(45, space).setSize(250, 16) ;
    space+=35; 
     cp5.addSlider("pD").plugTo(parent, "pD").setRange(0, 1).setValue(.2).setPosition(45, space).setSize(250, 16) ;
    space+=35; 
     cp5.addSlider("iD").plugTo(parent, "iD").setRange(0, .3).setValue(.1).setPosition(45, space).setSize(250, 16) ;
    space+=35; 
     cp5.addSlider("dD").plugTo(parent, "dD").setRange(0, .1).setValue(.05).setPosition(45, space).setSize(250, 16) ;
    space+=35; 
    //-------------CAMERA
    cp5.addBang("saveSet").setPosition(45, space).setSize(21, 21).setLabel(" SAVE SETTINGS ");
        space+=105;
  }
  /*    cp5.addSlider("goToX").plugTo(parent,"goToX").setRange(0, 35).setPosition(10,space);
   space+=20;
   cp5.addSlider("goToY").plugTo(parent,"goToY").setRange(0, 35).setPosition(10,space);
   space+=20;
   cp5.addSlider("goToZ").plugTo(parent,"goToZ").setRange(0, 35).setPosition(10,space);
   */
   void toggle(boolean theFlag) {
  if(theFlag==true) {
    motorsActive = 1;
  } else {
    motorsActive = 0;
  }
  println("a toggle event.");
}


public void updater(){
 texts();
}

  public void controlEvent(ControlEvent theEvent) {
    
    for (int i=-15; i<16; i+=5) {
      if (theEvent.getController().getName().equals("lineA"+i)) {
        println("lineA"+(i*abs(i)));
        lineO+=(i*abs(i))*jogK;
        CalcXYZ();
      } else if (theEvent.getController().getName().equals("lineB"+i)) {
        lineP+=(i*abs(i))*jogK;
        CalcXYZ();
      } else if (theEvent.getController().getName().equals("lineC"+i)) {
        lineQ+=(i*abs(i))*jogK;
        CalcXYZ();
      } else if (theEvent.getController().getName().equals("coorX"+i)) {
        pointR.x+=(i*abs(i))*jogK;
        CalcLineOPQ(pointR.x, pointR.y, pointR.z);
      } else if (theEvent.getController().getName().equals("coorY"+i)) {
        pointR.y+=(i*abs(i))*jogK;
        CalcLineOPQ(pointR.x, pointR.y, pointR.z);
      } else if (theEvent.getController().getName().equals("coorZ"+i)) {
        pointR.z+=(i*abs(i))*jogK;
        CalcLineOPQ(pointR.x, pointR.y, pointR.z);
      }
    }
    if (theEvent.getController().getName().equals("home")) {
      //  myPort.write("H");
      print("home:");
      pointR.x=pointH.x;
      pointR.y=pointH.y;
      pointR.z=pointH.z;
    } else if (theEvent.getController().getName().equals("setHome")) {
      //   myPort.write("V");
      println("setHome:");
      pointH.x=pointR.x;
      pointH.y=pointR.y;
      pointH.z=pointR.z;
    } else if (theEvent.getController().getName().equals("stop")) {
      if(stop>0){ stop=0; println("stop->GO");}
      else{ stop=1; println("STOP");}
     
    } else if (theEvent.getController().getName().equals("saveSet")) {
      saveSettings();
    }
     sendXYZ();
     sendPID();
  }

  
void VK_PAGE_UP (){ pointR.x+=10;}
void VK_PAGE_DOWN (){ pointR.x-=10;}

  
void keyPressed(){
 if (key == CODED) {
    if (keyCode == UP) { pointR.y+=10;}
    if (keyCode == DOWN) { pointR.y-=10;}
    if (keyCode == LEFT) { pointR.x-=10;}
    if (keyCode == RIGHT) { pointR.x+=10;}
    }
}

  public void draw() {
    background(abc);
  }

  private ControlFrame() {
  }

  public ControlFrame(Object theParent, int theWidth, int theHeight) {
    parent = theParent;
    w = theWidth;
    h = theHeight;
  }


  public ControlP5 control() {
    return cp5;
  }


  ControlP5 cp5;

  Object parent;
}
