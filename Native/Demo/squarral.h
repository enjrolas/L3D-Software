#include "neopixel/neopixel.h"
#include "datatypes.h"
#include "cube.h"

#ifndef squarral_h
#define squarral_h

//function definitions
color lerpColor(color* a, color* b, int val, int min, int max);
color colorMap(float val, float min, float max);
//adds two 3D vectors together and saves the result into vector a
void add(point& a, point& b);
void initSquarral();
void squarral(Adafruit_NeoPixel& strip);

#endif
