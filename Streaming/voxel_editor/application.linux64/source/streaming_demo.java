import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import peasy.*; 
import hypermedia.net.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class streaming_demo extends PApplet {



int [][][] cube;
int side=8;
PVector center=new PVector(side/2-.5f, side/2 - 0.5f, side/2- 0.5f);
PeasyCam cam;
int scale=240;
int state=0;
int zed=0;
int inc=1;
Point spot=new Point(0,0,0);
PImage logo;

public void setup()
{
  logo=loadImage("logo.png");
  cube=new int[side][side][side];
  print(center);
  size(displayWidth, displayHeight, P3D);
  cam = new PeasyCam(this, 4000);
  cam.setMinimumDistance(500);
  cam.setMaximumDistance(10000);
  initMulticast();
  frameRate(100);
}


public void draw()
{
  background(0); 
  rotateX(PI);
  stroke(255, 10);
  translate(-side*scale/2, -side*scale/2, -side*scale/2);
  updateCube();
  
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
      
cam.beginHUD();
image(logo,0,0);
String []info={"L3D Cube streaming test",
                "This program streams to any cube on the local network that's running the Listening program",
                "Move the blue voxel around using the arrow keys for the x and y axes",
                "The 'q' and 'a' keys move it along the z axis",
                "Voxel position: ("+spot.x+", "+spot.y+", "+spot.z+")"};

HUDText(info, new PVector(0,100,0));
cam.endHUD();
      
}

public void HUDText(String[] info, PVector origin)
{
  fill(255);
  for(int i=0;i<info.length;i++)
    text(info[i],origin.x, origin.y+15*i, origin.z);
}

public void setVoxel(int x, int y, int z, int col)
{
  if ((x>=0)&&(x<side))
    if ((y>=0)&&(y<side))
      if ((z>=0)&&(z<side))
        cube[x][y][z]=col;
}

public void setVoxel(Point p, int col)
{
  if ((p.x>=0)&&(p.x<side))
    if ((p.y>=0)&&(p.y<side))
      if ((p.z>=0)&&(p.z<side))
        cube[p.x][p.y][p.z]=col;
}

public void updateCube()
{
  cubeBackground(color(0, 0, 0)); 
  frame();
  oneVoxel();
  sendData();
  //delay(10);
}

public void cubeBackground(int col)
{
  for (int x=0; x<side; x++)
    for (int y=0; y<side; y++)
      for (int z=0; z<side; z++)
        cube[x][y][z]=col;
}

public void oneVoxel()
{
  setVoxel(spot, color(0,0,255));
  fill(255);
  text("( "+spot.x+", "+spot.y+", "+spot.z+" )",10,10);
}

public void frame()
{
  int col=colorMap(frameCount%1000, 0, 1000);
  Point p1=new Point(3, 0, zed);
  Point p2=new Point(7, 4, zed);
  Point p3=new Point(4, 7, zed);
  Point p4=new Point(0, 3, zed);

  switch(state%4) {
    case(0):
    line(p1, p2, col);
    break;
    case(1):
    line(p2, p3, col);
    break;
    case(2):
    line(p3, p4, col);
    break;
    case(3):
    line(p4, p1, col);
    break;
  }
  if ((frameCount%4)==0)
  {
    state++;
    if (state%4==0)
    {

      zed+=inc;
      if ((zed==side-1)||(zed==0))
        inc*=-1;
    }
  }
}


public void line(Point p1, Point p2, int col) {

  int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc, err_1, err_2, dx2, dy2, dz2;
  Point currentPoint=new Point(p1.x, p1.y, p1.z);
  dx = p2.x - p1.x;
  dy = p2.y - p1.y;
  dz = p2.z - p1.z;
  x_inc = (dx < 0) ? -1 : 1;
  l = abs(dx);
  y_inc = (dy < 0) ? -1 : 1;
  m = abs(dy);
  z_inc = (dz < 0) ? -1 : 1;
  n = abs(dz);
  dx2 = l << 1;
  dy2 = m << 1;
  dz2 = n << 1;

  if ((l >= m) && (l >= n)) {
    err_1 = dy2 - l;
    err_2 = dz2 - l;
    for (i = 0; i < l; i++) {
      setVoxel(currentPoint, col);
      if (err_1 > 0) {
        currentPoint.y += y_inc;
        err_1 -= dx2;
      }
      if (err_2 > 0) {
        currentPoint.z += z_inc;
        err_2 -= dx2;
      }
      err_1 += dy2;
      err_2 += dz2;
      currentPoint.x += x_inc;
    }
  } else if ((m >= l) && (m >= n)) {
    err_1 = dx2 - m;
    err_2 = dz2 - m;
    for (i = 0; i < m; i++) {
      setVoxel(currentPoint, col);
      if (err_1 > 0) {
        currentPoint.x += x_inc;
        err_1 -= dy2;
      }
      if (err_2 > 0) {
        currentPoint.z += z_inc;
        err_2 -= dy2;
      }
      err_1 += dx2;
      err_2 += dz2;
      currentPoint.y += y_inc;
    }
  } else {
    err_1 = dy2 - n;
    err_2 = dx2 - n;
    for (i = 0; i < n; i++) {
      setVoxel(currentPoint, col);
      if (err_1 > 0) {
        currentPoint.y += y_inc;
        err_1 -= dz2;
      }
      if (err_2 > 0) {
        currentPoint.x += x_inc;
        err_2 -= dz2;
      }
      err_1 += dy2;
      err_2 += dx2;
      currentPoint.z += z_inc;
    }
  }

  setVoxel(currentPoint, col);
}

class Point {
  int x, y, z;

  Point() {
  }
  Point(int _x, int _y, int _z)
  {
    x=_x;
    y=_y;
    z=_z;
  }
}

public int colorMap(float val, float min, float max)
{
  float range=1024;
  val=map(val, min, max, 0, range);
  int colors[]=new int[6];
  colors[0]=color(0, 0, 255);
  colors[1]=color(0, 255, 255);
  colors[2]=color(0, 255, 0);
  colors[3]=color(255, 255, 0);
  colors[4]=color(255, 0, 0);
  colors[5]=color(255, 0, 255);
  if (val<=range/6)
  {
    return(lerpColor(colors[0], colors[1], val/(range/6)));
  } else if (val<=2*range/6)
    return(lerpColor(colors[1], colors[2], (val/(range/6))-1));
  else if (val<=3*range/6)
    return(lerpColor(colors[2], colors[3], (val/(range/6))-2));
  else if (val<=4*range/6)
    return(lerpColor(colors[3], colors[4], (val/(range/6))-3));
  else if (val<=5*range/6)
    return(lerpColor(colors[4], colors[5], (val/(range/6))-4));
  else
    return(lerpColor(colors[5], colors[0], (val/(range/6))-5));
}

public void keyPressed()
{
  if(keyCode==LEFT)
  {
    spot.x--;
    if(spot.x<0)
      spot.x=side-1;
  }
  if(keyCode==RIGHT)
  {
    spot.x++;
    if(spot.x>=side)
      spot.x=0;
    }
  if(keyCode==UP)
  {
    spot.y++;
    if(spot.y>=side)
      spot.y=0;
    }
  if(keyCode==DOWN)
  {
    spot.y--;
    if(spot.y<0)
      spot.y=side-1;
    }

  if(key=='q')
  {
    spot.z++;
    if(spot.z>=side)
      spot.z=0;
    }
  if(key=='a')
  {
    spot.z--;
    if(spot.z<0)
      spot.z=side-1;
    }
}

// import UDP library

UDP udp;  // define the UDP object
String ip       = "224.0.0.1";  // the remote IP address
int port        = 6000;    // the destination port
int multicastPixels=170;

public void initMulticast()
{
  // create a new datagram connection on port 6000
  // and wait for incomming message
  // create a multicast connection on port 6000
  // and join the group at the address "224.0.0.1"
  udp = new UDP( this, port, ip );
  // ... well, just verifies if it's really a multicast socket and blablabla
  println( "init as multicast socket ... "+udp.isMulticast() );
  println( "joins a multicast group  ... "+udp.isJoined() );
  //udp.log( true );     // <-- printout the connection activity
}

public void sendData()
{
  int index=0;
  byte[] data=new byte[(int)pow(side, 3)];
  for (int z=0; z<side; z++)
    for (int y=0; y<side; y++)
      for (int x=0; x<side; x++)
      {
        index=z*64+y*8+x;
        data[index]=colorByte(cube[x][y][z]);
      }
  udp.send( data);
}

public byte colorByte(int col)
{
  return (byte)((int)red(col)&224 | ((int)green(col)&224)>>3 | ((int)blue(col)&192)>>6);
}

  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "--full-screen", "--bgcolor=#666666", "--stop-color=#cccccc", "streaming_demo" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
