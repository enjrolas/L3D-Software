import peasy.*;

color [][][] cube;
int side=8;
PeasyCam cam;
int scale=120;
int gridScale=30;
int state=0;
int zed=0;
int inc=1;
Point spot=new Point(0, 0, 0);
int wallPlane=1;
float hingePos=1;
float doorWidth=4;
float doorHeight=6;
float angle=0;
float angleInc=0.25;
PImage logo;
int selX, selY;
color doorColor=color(255, 0, 0);
color wallColor=color(255, 255, 0);
int layer=0;
void setup()
{
  logo=loadImage("logo.png");
  cube=new color[side][side][side];
  size(displayWidth, displayHeight, P3D);
  cam = new PeasyCam(this, 4000);
  cam.setMinimumDistance(500);
  cam.setMaximumDistance(10000);
  cam.rotateX(PI/8);
  cam.rotateY(PI/8);
  initMulticast();
}


void draw()
{
  background(0); 
  draw3DPreview();
  drawLayer(50, 100);
  updateCube();
}

void draw3DPreview()
{
  rotateX(PI);
  stroke(255, 10);
  translate(width, 0);
  translate(-side*scale/2, -side*scale/2, -side*scale/2);
  for (int x=0; x<side; x++)  
    for (int y=0; y<side; y++)  
      for (int z=0; z<side; z++)  
      {
        pushMatrix();
        translate(x*scale, y*scale, (side-1-z)*scale);
        if ((x==selX)&&(z==selY)&&(y==layer))
        {
          strokeWeight(4);
          stroke(255);
        } else
          noStroke();
        if (brightness(cube[x][y][z])!=0)
        {
          if (y==layer)
          {
            fill(cube[x][y][z]);
          } else
            fill(red(cube[x][y][z]), green(cube[x][y][z]), blue(cube[x][y][z]), 100);
        } else
        {
          noFill();
          if (y==layer)
          {
            strokeWeight(1);
            stroke(255, 50);
          }
        }
        box(scale, scale, scale);
        popMatrix();
      }
}

void drawLayer(int xOffset, int yOffset)
{
  cam.beginHUD();
  pushMatrix();
  translate(xOffset, yOffset);
  for (int x=0;x<side;x++)
    for (int y=0;y<side;y++)
    {
      if ((x==selX)&&(y==selY))
      {
        stroke(255);
        strokeWeight(3);
      } else {
        stroke(100);
        strokeWeight(2);
      }
      pushMatrix();
      translate(x*gridScale, y*gridScale);
      fill(cube[x][layer][y]);
      rect(x, y, gridScale, gridScale);
      popMatrix();
    } 
  popMatrix();

  cam.endHUD();
}


void HUDText(String[] info, PVector origin)
{
  fill(255);
  for (int i=0;i<info.length;i++)
    text(info[i], origin.x, origin.y+15*i, origin.z);
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
  frame();
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
  //draw the wall
  for (int x=0;x<hingePos;x++)
    line(new Point(x, 0, wallPlane), new Point(x, side-1, wallPlane), wallColor);
  for (float x=hingePos;x<hingePos+doorWidth+1;x++)
    line(new Point(x, doorHeight+1, wallPlane), new Point(x, side-1, wallPlane), wallColor);

  for (float x=hingePos+doorWidth+1;x<side;x++)
    line(new Point(x, 0, wallPlane), new Point(x, side-1, wallPlane), wallColor);

  Point doorPos=new Point(hingePos+doorWidth*cos(angle), wallPlane+doorWidth*sin(angle));
  for (int i=0;i<doorHeight+1;i++)
    line(new Point(hingePos, i, wallPlane), new Point(doorPos.x, i, doorPos.y), color(255, 0, 0));
  Point knockerPos=new Point(hingePos+doorWidth*cos(angle+.2), 2, wallPlane+doorWidth*sin(angle+.2));
  setVoxel(knockerPos, color(0, 0, 255));
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
  if (key=='q')
  {
    layer++;
    if (layer>=side)
      layer=0;
  }
  if (key=='a')
  {
    layer--;
    if (layer<0)
      layer=side-1;
  }
  if (key==CODED)
    switch(keyCode) {
      case(DOWN):
      selY++;
      if (selY>=side)
        selY=0;
      break;
      case(UP):
      selY--;
      if (selY<0)
        selY=side-1;
      break;
      case(RIGHT):
      selX++;
      if (selX>=side)
        selX=0;
      break;
      case(LEFT):
      selX--;
      if (selX<0)
        selX=side-1;
      break;
    }
}

