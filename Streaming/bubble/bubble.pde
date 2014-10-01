import peasy.*;

import com.leapmotion.leap.Controller;
import com.leapmotion.leap.Frame;
import com.leapmotion.leap.Hand;
import com.leapmotion.leap.HandList;
import com.leapmotion.leap.Vector;
import com.leapmotion.leap.processing.LeapMotion;

LeapMotion leapMotion;

color [][][] cube;
int side=8;
PVector center=new PVector(side/2-.5, side/2 - 0.5, side/2- 0.5);
PeasyCam cam;
int scale=240;

Vector handPosition, min, max;
PVector handVoxel;
boolean drawBubbles=false;

PImage logo;

void setup()
{
  logo=loadImage("logo.png");
  initMulticast();
  cube=new color[side][side][side];
  print(center);
  size(displayWidth, displayHeight, P3D);
  cam = new PeasyCam(this, 3000);
  cam.setMinimumDistance(500);
  cam.setMaximumDistance(10000);
  initBubbles();
  // stroke(255,20);
  //noStroke();
  leapMotion = new LeapMotion(this);
  handPosition=new Vector(0, 0, 0);
  handVoxel=new PVector(-1, -1, -1);
  min=new Vector(1000, 1000, 1000);
  max=new Vector(-1000, -1000, -1000);
}

void draw()
{
  background(0); 
  rotateX(PI);
  stroke(255, 10);
  updateCube();
  translate(-side*scale/2, -side*scale/2, -side*scale/2);
  for (int x=0; x<side; x++)  
    for (int y=0; y<side; y++)  
      for (int z=0; z<side; z++)  
      {
        pushMatrix();
        translate(x*scale, y*scale, z*scale);
        if (brightness(cube[x][y][z])!=0)
          fill(cube[x][y][z]);
        else
          noFill();
        box(scale, scale, scale);
        popMatrix();
      }
      helpMenu();
}

void updateCube()
{
  updateBubbles();
  setVoxel((int)handVoxel.x, (int)handVoxel.y, (int)handVoxel.z, color(0, 255, 0));
  sendData();
}

void onFrame(final Controller controller)
{
  Frame frame = controller.frame();
  HandList allHands=frame.hands();
  if (!allHands.isEmpty())
  {
    Hand firstHand=allHands.get(0);
    handPosition=firstHand.palmPosition();
  }
  if (handPosition.getX()<min.getX())
    min.setX(handPosition.getX());
  if (handPosition.getY()<min.getY())
    min.setY(handPosition.getY());
  if (handPosition.getZ()<min.getZ())
    min.setZ(handPosition.getZ());

  if (handPosition.getX()>max.getX())
    max.setX(handPosition.getX());
  if (handPosition.getY()>max.getY())
    max.setY(handPosition.getY());
  if (handPosition.getZ()>max.getZ())
    max.setZ(handPosition.getZ());

  handVoxel=new PVector(map(handPosition.getX(), min.getX(), max.getX(), 0, side-1), map(handPosition.getY(), min.getY(), max.getY(), 0, side-1), map(handPosition.getZ(), min.getZ(), max.getZ(), 0, side-1));
  println((int)handVoxel.x+", "+(int)handVoxel.y+", "+(int)handVoxel.z);
}


void helpMenu()
{
  cam.beginHUD();
  image(logo, 0, 0);
  String []info= {
    "L3D bubble ninja", 
    "This program streams to any cube on the local network that's running the Listening program", 
    "Bubbles will bubble up from the bottom of the cube.  Move your hand over a Leap Motion controller -- it will appear in a cube",
    "as a green laser", 
    "Use the laser to pop all the bubbles!", 
    "hand position: ("+(int)handVoxel.x+", "+(int)handVoxel.y+", "+(int)handVoxel.z+")"
  };

  HUDText(info, new PVector(0, 100, 0));
  cam.endHUD();
}

void HUDText(String[] info, PVector origin)
{
  fill(255);
  for (int i=0; i<info.length; i++)
    text(info[i], origin.x, origin.y+15*i, origin.z);
}

