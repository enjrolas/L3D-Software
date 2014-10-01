#include "colors.h"
//maxBrightness is the brightness limit for each pixel.  All color data will be scaled down 
//so that the largest value is maxBrightness
int maxBrightness=50;


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
