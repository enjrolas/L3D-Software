//#include "neopixel/neopixel.h"
//#include "datatypes.h"
#include "cube.h"

color black;

//sets a pixel at position (x,y,z) to the col parameter's color
void setPixel(int x, int y, int z, Adafruit_NeoPixel& strip, color* col)
{
  int index = (z*SIDE*SIDE) + (x*SIDE) + y;
  int rando=10;
  if(col->green>rando/2)
    strip.setPixelColor(index,strip.Color(col->red,  rand()%rando - rando/2 + col->green, col->blue));
  else
    strip.setPixelColor(index,strip.Color(col->red,  col->green, col->blue));
    
}


//returns the color value currently displayed at the x,y,z location
color getPixel(int x, int y, int z, Adafruit_NeoPixel& strip)
{
  int index = (z*SIDE*SIDE) + (x*SIDE) + y;
  uint32_t col=strip.getPixelColor(index);
  color pixelColor;
  pixelColor.red=(col>>16)&255;
  pixelColor.green=(col>>8)&255;
  pixelColor.blue=col&255;
  return pixelColor;
}

void initCube()
{
  black.red=0;
  black.green=0;
  black.blue=0;
  //strip=Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
}


void blackBackground(Adafruit_NeoPixel& strip)
{
  for(int x=0;x<SIDE;x++)
    for(int y=0;y<SIDE;y++)
      for(int z=0;z<SIDE;z++)
	setPixel(x,y,z, strip, &black);    
}
