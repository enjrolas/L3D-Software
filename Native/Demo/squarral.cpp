#include "squarral.h"
#define TRAIL_LENGTH 50


int frame=0;
color pixelColor;
point position, increment, pixel;
point trailPoints[TRAIL_LENGTH];
int posX, posY, posZ;
int incX, incY, incZ;
int squarral_zInc=1;
int bound=0;
int boundInc=1;
unsigned char axis=0;
bool rainbow=true;

//maxBrightness is the brightness limit for each pixel.  All color data will be scaled down 
//so that the largest value is maxBrightness
int maxBrightness=50;

void initSquarral() 
{
  position={0,0,0};
  increment={1,0,0};
}

void squarral(Adafruit_NeoPixel& strip) 
{
  add(position, increment);
  if((increment.x==1)&&(position.x==SIDE-1-bound))
    increment={0,1,0};
  if((increment.x==-1)&&(position.x==bound))
    increment={0,-1,0};
  if((increment.y==1)&&(position.y==SIDE-1-bound))
    increment={-1,0,0};
  if((increment.y==-1)&&(position.y==bound))
    {
      increment={1,0,0};
      position.z+=squarral_zInc;
      bound+=boundInc;
      if((position.z==3)&&(squarral_zInc>0))
	boundInc=0;
      if((position.z==4)&&(squarral_zInc>0))
	boundInc=-1;
      if((position.z==3)&&(squarral_zInc<0))
	boundInc=-1;
      if((position.z==4)&&(squarral_zInc<0))
	boundInc=0;
        
      if((position.z==0)||(position.z==SIDE-1))
	boundInc*=-1;
            
      if((position.z==SIDE-1)||(position.z==0))
        {
	  squarral_zInc*=-1;
	  if(squarral_zInc==1)
            {
	      axis=rand()%6;
	      if(rand()%10==0)
		rainbow=true;
	      else
		rainbow=false;
            }
        }
    }
    
  posX=position.x;
  posY=position.y;
  posZ=position.z;
    
  incX=increment.x;
  incY=increment.y;
  incZ=increment.z;
    
  for(int i=TRAIL_LENGTH-1;i>0;i--)
    {
      trailPoints[i].x=trailPoints[i-1].x;
      trailPoints[i].y=trailPoints[i-1].y;
      trailPoints[i].z=trailPoints[i-1].z;
    }
  trailPoints[0].x=pixel.x;
  trailPoints[0].y=pixel.y;
  trailPoints[0].z=pixel.z;
  switch(axis){
  case(0):
    pixel.x=position.x;
    pixel.y=position.y;
    pixel.z=position.z;
    break;
  case(1):
    pixel.x=position.z;
    pixel.y=position.x;
    pixel.z=position.y;
    break;
  case(2):
    pixel.x=position.y;
    pixel.y=position.z;
    pixel.z=position.x;
    break;
  case(3):
    pixel.x=position.z;
    pixel.y=SIDE-1-position.x;
    pixel.z=position.y;
    break;
  case(4):
    pixel.x=position.y;
    pixel.y=position.z;
    pixel.z=SIDE-1-position.x;
    break;
  case(5):
    pixel.x=position.x;
    pixel.y=SIDE-1-position.y;
    pixel.z=position.z;
    break;
  }
        
  pixelColor=colorMap(frame%1000,0,1000);
  setPixel((int)pixel.x, (int)pixel.y, (int)pixel.z, strip, &pixelColor);
  for(int i=0;i<TRAIL_LENGTH;i++)
    {
      color trailColor;
      if(rainbow)
        {
	  trailColor=colorMap((frame+(i*1000/TRAIL_LENGTH))%1000,0,1000);
	  //fade the trail to black over the length of the trail
	  trailColor.red=trailColor.red*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
	  trailColor.green=trailColor.green*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
	  trailColor.blue=trailColor.blue*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
        }
      else
        {
	  trailColor.red=pixelColor.red*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
	  trailColor.green=pixelColor.green*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
	  trailColor.blue=pixelColor.blue*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
        }
      setPixel((int)trailPoints[i].x, (int)trailPoints[i].y, (int)trailPoints[i].z, strip, &trailColor);
    }
  frame++;
}

void add(point& a, point& b)
{
  a.x+=b.x;
  a.y+=b.y;
  a.z+=b.z;
}


//returns a color from a set of colors fading from blue to green to red and back again
//the color is returned based on where the parameter *val* falls between the parameters
//*min* and *max*.  If *val* is min, the function returns a blue color.  If *val* is halfway
//between *min* and *max*, the function returns a yellow color.  
color colorMap(float val, float min, float max)
{
  float range=1024;
  val=range*(val-min)/(max-min);
  color colors[6];
  colors[0].red=0;
  colors[0].green=0;
  colors[0].blue=maxBrightness;
  
  colors[1].red=0;
  colors[1].green=maxBrightness;
  colors[1].blue=maxBrightness;
  
  colors[2].red=0;
  colors[2].green=maxBrightness;
  colors[2].blue=0;
  
  colors[3].red=maxBrightness;
  colors[3].green=maxBrightness;
  colors[3].blue=0;
  
  colors[4].red=maxBrightness;
  colors[4].green=0;
  colors[4].blue=0;

  colors[5].red=maxBrightness;
  colors[5].green=0;
  colors[5].blue=maxBrightness;
  
  if (val<=range/6)
    return(lerpColor(&colors[0], &colors[1], val, 0, range/6));
  else if (val<=2*range/6)
    return(lerpColor(&colors[1], &colors[2], val, range/6, 2*range/6));
  else if (val<=3*range/6)
    return(lerpColor(&colors[2], &colors[3], val, 2*range/6, 3*range/6));
  else if (val<=4*range/6)
    return(lerpColor(&colors[3], &colors[4], val, 3*range/6, 4*range/6));
  else if (val<=5*range/6)
    return(lerpColor(&colors[4], &colors[5], val, 4*range/6, 5*range/6));
  else
    return(lerpColor(&colors[5], &colors[0], val, 5*range/6, range));
}


//returns a color that's an interpolation between colors a and b.  The color
//is controlled by the position of val relative to min and max -- if val is equal to min,
//the resulting color is identical to color a.  If it's equal to max, the resulting color 
//is identical to color b.  If val is (max-min)/2, the resulting color is the average of
//color a and color b
color lerpColor(color* a, color* b, int val, int min, int max)
{
  color lerped;
  lerped.red=a->red+(b->red-a->red)*(val-min)/(max-min);
  lerped.green=a->green+(b->green-a->green)*(val-min)/(max-min);
  lerped.blue=a->blue+(b->blue-a->blue)*(val-min)/(max-min);
  return lerped;
}
