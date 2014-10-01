#include "neopixel/neopixel.h"
#include "datatypes.h"
#define SIDE 8
//set up the pin that controls the LEDs, the type of LEDs (WS2812B) and the number of LEDs in the cube (8*8*8=512)
#define PIXEL_PIN D0
#define PIXEL_COUNT 512
#define PIXEL_TYPE WS2812B

//sets a pixel at position (x,y,z) to the col parameter's color
void setPixel(int x, int y, int z, Adafruit_NeoPixel& strip, color* col);
//returns the color value currently displayed at the x,y,z location
color getPixel(int x, int y, int z, Adafruit_NeoPixel& strip);
//void setPixel(int x, int y, int z, color* col);

//initializes the cube's LEDs and sets everything to black
void initCube();
void blackBackground(Adafruit_NeoPixel& strip);
