import peasy.*;

color [][][] cube;
int side=8;
PVector center=new PVector(side/2-.5, side/2 - 0.5, side/2- 0.5);
PeasyCam cam;
int scale=240;
PImage logo;
Bubble spot;
float inc=0.25;
int state=0;
PVector incVector=new PVector(inc,0,0);
int numStates=9;
Point[] trail;
color spotCol=color(0,255,255);
String coreName="";
void setup()
{
  spot=new Bubble(0,0,0,1,spotCol);
  trail=new Point[50];
  for(int i=0;i<trail.length;i++)
    trail[i]=new Point();
  logo=loadImage("logo.png");
  cube=new color[side][side][side];
  print(center);
  println("getting IP address...");
  size(displayWidth, displayHeight, P3D);
  cam = new PeasyCam(this, 6000);
  cam.setMinimumDistance(500);
  cam.setMaximumDistance(10000);
  cam.rotateX(PI/8);
  cam.rotateY(PI/8);
  initUI();
  initDirectStreaming(coreName);
  frameRate(100);
  println(getNames());
}

void draw()
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
        translate(x*scale, y*scale, (side-1-z)*scale);
        if (brightness(cube[x][y][z])!=0)
          fill(cube[x][y][z]);
        else
          noFill();
        box(scale, scale, scale);
        popMatrix();
      }
      
cam.beginHUD();
image(logo,0,0);
String []info={"L3D Cube direct streamer",
                "This program finds all cubes associated with your spark access token,",
                "and can selectively stream a visualization to any cube running the listener program on your local network.",
                "To use it on your own cube, just enter your spark access token below",
                "Now streaming to cube: "+coreName +" at IP address "+address+" on port "+port,
                "select a core:",
};
cp5.draw();
HUDText(info, new PVector(0,100,0));
cam.endHUD();
      
}

void HUDText(String[] info, PVector origin)
{
  fill(255);
  for(int i=0;i<info.length;i++)
    text(info[i],origin.x, origin.y+15*i, origin.z);
}

void setVoxel(int x, int y, int z, color col)
{
  if ((x>=0)&&(x<side))
    if ((y>=0)&&(y<side))
      if ((z>=0)&&(z<side))
        cube[x][y][z]=col;
}

void setVoxel(Point p, color col)
{
  if ((p.x>=0)&&(p.x<side))
    if ((p.y>=0)&&(p.y<side))
      if ((p.z>=0)&&(p.z<side))
        cube[p.x][p.y][p.z]=col;
}

void updateCube()
{
  cubeBackground(color(0, 0, 0));
  if((frameCount%20)<10)
  {
    spot.drawPoint();
   // spot.drawBubble();
  }
  if(millis()>1000) //don't move for the first two seconds
  spot.center.add(incVector);
  switch(state%numStates){
    case(0):
      if(spot.center.x>=side-1)
      {
        state++;
        incVector=new PVector(0,inc,0);
      }
    break;
    case(1):
      if(spot.center.y>=side-1)
      {
        state++;
        incVector=new PVector(-inc,0,0);
      }
    break;
    case(2):
      if(spot.center.x<=0)
      {
        state++;
        incVector=new PVector(0,-inc,0);
      }
    break;
    case(3):
      if(spot.center.y<=0)
      {
        state++;
//        incVector=new PVector(inc,0,0);
        incVector=new PVector(inc,0,inc);
      }
    break;
    case(4):
      if(spot.center.z>=side-1)
      {
        state++;
        incVector=new PVector(0,inc,0);
      }
    break;
    case(5):
      if(spot.center.y>=side-1)
      {
        state++;
        incVector=new PVector(0,0,-inc);
      }
    break;
    case(6):
      if(spot.center.z<=0)
      {
        state++;
        incVector=new PVector(-inc,0,0);
      }
    break;
    case(7):
      if(spot.center.x<=0)
      {
        state++;
        incVector=new PVector(0,-inc,0);
      }
    break;
    case(8):
      if(spot.center.y<=0)
      {
        state++;
        incVector=new PVector(inc,0,0);
      }
    break;
  }
  for(int i=trail.length-2;i>=0;i--)
    trail[i+1]=trail[i];
  trail[0]=new Point(round(spot.center.x),round(spot.center.y),round(spot.center.z));
  for(int i=0;i<trail.length;i++)
    setVoxel(trail[i], color(map(i,0, trail.length-1,red(spotCol), 0),map(i,0, trail.length-1,green(spotCol), 0),map(i,0, trail.length-1,blue(spotCol), 0)));  //fade the trail to black over its lenght
  sendData();
  //delay(10);
}

void cubeBackground(color col)
{
  for (int x=0; x<side; x++)
    for (int y=0; y<side; y++)
      for (int z=0; z<side; z++)
        cube[x][y][z]=col;
}

void mouseClicked()
{
}



class Point {
  int x, y, z;

  Point() {
  }
  Point(int _x, int _y)
  {
    x=_x;
    y=_y;
    z=0;
  }
  Point(float _x, float _y)
  {
    x=(int)round(_x);
    y=(int)round(_y);
    z=0;
  }
  Point(int _x, int _y, int _z)
  {
    x=_x;
    y=_y;
    z=_z;
  }
  Point(float _x, float _y, float _z)
  {
    x=(int)round(_x);
    y=(int)round(_y);
    z=(int)round(_z);
  }
}

color colorMap(float val, float min, float max)
{
  float range=1024;
  val=map(val, min, max, 0, range);
  color colors[]=new color[6];
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

void keyPressed()
{
}
