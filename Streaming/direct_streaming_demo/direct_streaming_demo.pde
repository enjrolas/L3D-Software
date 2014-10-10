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
String ipAddress;
int port=6666;
void setup()
{
  spot=new Bubble(0,0,0,1,spotCol);
  trail=new Point[50];
  for(int i=0;i<trail.length;i++)
    trail[i]=new Point();
  logo=loadImage("logo.png");
  cube=new color[side][side][side];
  print(center);
  loadCores("99545512f1ba12068e3678cd079cd9dc26855863");
  ipAddress=getVar("delilah", "IPAddress");  
  //ipAddress="192.168.254.189";
  size(displayWidth, displayHeight, P3D);
  cam = new PeasyCam(this, 6000);
  cam.setMinimumDistance(500);
  cam.setMaximumDistance(10000);
  cam.rotateX(PI/8);
  cam.rotateY(PI/8);
//  initDirectStreaming(ipAddress, port);
  initDirectStreaming(ipAddress);
  frameRate(100);
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
String []info={"L3D Cube door simulator",
                "This program streams to any cube on the local network that's running the Listening program",
                "This shows a simple door opening and closing",
                "use the left and right arrow keys to open or close the door",
                "streaming to "+ipAddress+" on port "+port,
};
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


void frame()
{

}


void line(Point p1, Point p2, color col) {

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
