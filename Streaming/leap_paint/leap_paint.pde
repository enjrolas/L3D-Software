import peasy.*;

import java.util.Map;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.ConcurrentHashMap;

import com.leapmotion.leap.Controller;
import com.leapmotion.leap.Finger;
import com.leapmotion.leap.Frame;
import com.leapmotion.leap.Hand;
import com.leapmotion.leap.Tool;
import com.leapmotion.leap.Vector;
import com.leapmotion.leap.processing.LeapMotion;

LeapMotion leapMotion;
PeasyCam cam;

PImage logo;

color [][][] cube;
int side=8;
PVector center=new PVector(side/2-.5, side/2 - 0.5, side/2- 0.5);
int scale=240;


ConcurrentMap<Integer, Integer> toolColors;
ConcurrentMap<Integer, Vector> fingerPositions;
ConcurrentMap<Integer, Vector> toolPositions;
PVector min, max;
PVector[] fingerData;
PVector[][] trails;
color[] fingerColors;
boolean dataPresent=false;
int trailLength=50;

void setup()
{
  size(displayWidth, displayHeight,P3D);
  logo=loadImage("logo.png");
  initMulticast();
  cube=new color[side][side][side];
  trails=new PVector[5][trailLength];
  for(int i=0;i<5;i++)
    for(int j=0;j<trailLength;j++)
      trails[i][j]=new PVector();
  cam = new PeasyCam(this, 3000);
  cam.setMinimumDistance(500);
  cam.setMaximumDistance(10000);
  cam.rotateX(PI/8);
  cam.rotateY(-PI/8);
  fingerData=new PVector[5];
  fingerColors=new color[5];
  
  fingerColors[0]=color(255,0,0);
  fingerColors[1]=color(255,255,0);
  fingerColors[2]=color(0,255,0);
  fingerColors[3]=color(0,255,255);
  fingerColors[4]=color(0,0,255);
  
  for(int i=0;i<5;i++)
    fingerData[i]=new PVector();
  min=new PVector(1000,1000,1000);
  max=new PVector(-1000,-1000,-1000);
  background(20);
  frameRate(60);
  ellipseMode(CENTER);

  leapMotion = new LeapMotion(this);
  toolColors = new ConcurrentHashMap<Integer, Integer>();
  fingerPositions = new ConcurrentHashMap<Integer, Vector>();
  toolPositions = new ConcurrentHashMap<Integer, Vector>();
  println("finished setup");
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
   println("got through a draw loop");
      helpMenu();
   println("drew the help menu");
}

void updateCube()  
{
  fingerPaint();
  sendData();
}
  
  
void fingerPaint()
{
  singleColor(color(0));
  for (Map.Entry entry : fingerPositions.entrySet())
  {
    dataPresent=true;
    Integer fingerId = (Integer) entry.getKey();
    Vector position = (Vector) entry.getValue();
    noStroke();
//    ellipse(leapMotion.leapToSketchX(position.getX()), leapMotion.leapToSketchY(position.getY()), 24.0, 24.0);
//    println("finger: "+fingerId+"  position: "+leapMotion.leapToSketchX(position.getX())+", "+leapMotion.leapToSketchY(position.getY())+", "+leapMotion.leapToSketchZ(position.getZ()));
 //   println("finger: "+fingerId+"  position: "+position.getX()+", "+position.getY()+", "+position.getZ());
    if(position.getX()>max.x)
      max.x=position.getX();
    if(position.getY()>max.y)
      max.y=position.getY();
    if(position.getZ()>max.z)
      max.z=position.getZ();

    if(position.getX()<min.x)
      min.x=position.getX();
    if(position.getY()<min.y)
      min.y=position.getY();
    if(position.getZ()<min.z)
      min.z=position.getZ();
    
 //   println(fingerId%5);
    fingerData[fingerId%5].x=position.getX();
    fingerData[fingerId%5].y=position.getY();
    fingerData[fingerId%5].z=position.getZ();
  }
  for (Map.Entry entry : toolPositions.entrySet())
  {
    Integer toolId = (Integer) entry.getKey();
    Vector position = (Vector) entry.getValue();
    fill(toolColors.get(toolId));
    noStroke();
    ellipse(leapMotion.leapToSketchX(position.getX()), leapMotion.leapToSketchY(position.getY()), 24.0, 24.0);
 //   println("tool: "+toolId+"  position: "+position.getX()+", "+position.getY()+", "+position.getZ());
  }

  for(int i=0;i<5;i++)
    for(int j=trailLength-1;j>=1;j--  )
      trails[i][j]=trails[i][j-1].get();
      
  if(dataPresent)
  {
    for(int i=0;i<5;i++)
      trails[i][0]=new PVector( (byte)map(fingerData[i].x,min.x,max.x,0,7),
                                (byte)map(fingerData[i].y,min.y,max.y,0,7),
                                (byte)map(fingerData[i].z,min.z,max.z,0,7));
  }
  else
  {
    for(int i=0;i<5;i++)
      trails[i][0]=new PVector(-1,-1,-1);
  }
    
  dataPresent=false;
  fill(0,255,0);
  for(int i=0;i<5;i++)
    for(int j=0;j<trailLength;j++)
    {
//      color voxelColor=color(red(fingerColors[i]),green(fingerColors[i]),blue(fingerColors[i]),map(j,0,trailLength-1,100,0));
      color voxelColor=color(map(j,0,trailLength-1,red(fingerColors[i]),0),
                             map(j,0,trailLength-1,green(fingerColors[i]),0),
                             map(j,0,trailLength-1,blue(fingerColors[i]),0));
      if(trails[i][j].x!=-1)
        setVoxel((int)trails[i][j].x, (int)trails[i][j].y, (int)trails[i][j].z, voxelColor);
    }
}

void onFrame(final Controller controller)
{
  Frame frame = controller.frame();
  fingerPositions.clear();
  for (Finger finger : frame.fingers())
  {
    int fingerId = finger.id();
    color c = color(random(0, 255), random(0, 255), random(0, 255));
    fingerPositions.put(fingerId, finger.tipPosition());
  }
  toolPositions.clear();
  for (Tool tool : frame.tools())
  {
    int toolId = tool.id();
    color c = color(random(0, 255), random(0, 255), random(0, 255));
    toolColors.putIfAbsent(toolId, c);
    toolPositions.put(toolId, tool.tipPosition());
  }
  
}

void setVoxel(int x, int y, int z, color col)
{
  if ((x>=0)&&(x<side))
    if ((y>=0)&&(y<side))
      if ((z>=0)&&(z<side))
        cube[x][y][z]=col;
}

void singleColor(color col)
{
  for(int x=0;x<side;x++)
    for(int y=0;y<side;y++)
      for(int z=0;z<side;z++)
        cube[x][y][z]=col;
}


void helpMenu()
{
  cam.beginHUD();
  image(logo, 0, 0);
  String []info= {
    "L3D finger paint", 
    "Move your hand over a leap controller and paint with your fingers", 
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

