#include "fireworks.h"
#include "cube.h"
#include "datatypes.h"

int centerX, centerY, centerZ;
int launchX, launchZ;
int red, green, blue;
int brightness=255;
float radius=0;
float speed;
bool showRocket;
bool exploded;
float xInc, yInc, zInc;
float rocketX, rocketY, rocketZ;
float launchTime;
int maxSize;
color rocketColor, fireworkColor;

void updateFireworks(Adafruit_NeoPixel& strip)
{
  //loop through all the pixels, calculate the distance to the center point, and turn the pixel on if it's at the right radius
  for(int x=0;x<SIDE;x++)
    for(int y=0;y<SIDE;y++) 
      for(int z=0;z<SIDE;z++)
        {
	  if(showRocket)
	    if(abs(distance(x,y,z,rocketX, rocketY, rocketZ)-radius)<0.05)
	      setPixel(x,y,z,strip, &rocketColor);                
	  if(exploded)
	    if(abs(distance(x,y,z,centerX, centerY, centerZ)-radius)<0.1)
	      setPixel(x,y,z,strip, &fireworkColor);
        }

  if(exploded)
    radius+=speed;  //the sphere gets bigger
  if(showRocket)
    {
      rocketX+=xInc;
      rocketY+=yInc;
      rocketZ+=zInc;
    }
  //if our sphere gets too large, restart the animation in another random spot
  if(radius>maxSize)
    prepRocket();
  if(abs(distance(centerX,centerY,centerZ,rocketX, rocketY, rocketZ)-radius)<2)
    {
      showRocket=false;
      exploded=true;
    }
        
}

float distance(float x, float y, float z, float x1, float y1, float z1)
{
  return(sqrt(pow(x-x1,2)+pow(y-y1,2)+pow(z-z1,2)));
}

void prepRocket()
{
  radius=0;
  centerX=rand()%8;
  centerY=rand()%8;
  centerZ=rand()%8;
  fireworkColor.red=rand()%brightness;
  fireworkColor.green=rand()%brightness;
  fireworkColor.blue=rand()%brightness;
  launchX=rand()%8;
  launchZ=rand()%8;
  rocketX=launchX;
  rocketY=0;
  rocketZ=launchZ;
  launchTime=15+rand()%25;
  xInc=(centerX-rocketX)/launchTime;
  yInc=(centerY-rocketY)/launchTime;
  zInc=(centerZ-rocketZ)/launchTime;
  showRocket=true;
  exploded=false;
  speed=0.15;
  maxSize=2+rand()%6;
  //speed=rand()%5;
  //speed*=0.1;
}

void initFireworks()
{
  rocketColor.red=255;
  rocketColor.green=150;
  rocketColor.blue=100;
  prepRocket();
}
