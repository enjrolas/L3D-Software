#include "neopixel/neopixel.h"
#include "datatypes.h"
#include <math.h>
#include "cube.h"


float phase = 0.0;
float phaseIncrement = 0.035; // Controls the speed of the moving points. Higher == faster
float colorStretch = 0.23; // Higher numbers will produce tighter color bands 
float plasmaBrightness = 0.3;
color plasmaColor;


void zPlasma(Adafruit_NeoPixel& strip)
{
  
  phase += phaseIncrement;
  // The two points move along Lissajious curves, see: http://en.wikipedia.org/wiki/Lissajous_curve
  // We want values that fit the LED grid: x values between 0..8, y values between 0..8, z values between 0...8
  // The sin() function returns values in the range of -1.0..1.0, so scale these to our desired ranges.
  // The phase value is multiplied by various constants; I chose these semi-randomly, to produce a nice motion.
  point p1 = { (sin(phase*1.000)+1.0) * 4, (sin(phase*1.310)+1.0) * 4.0,  (sin(phase*1.380)+1.0) * 4.0};
  point p2 = { (sin(phase*1.770)+1.0) * 4, (sin(phase*2.865)+1.0) * 4.0,  (sin(phase*1.410)+1.0) * 4.0};
  point p3 = { (sin(phase*0.250)+1.0) * 4, (sin(phase*0.750)+1.0) * 4.0,  (sin(phase*0.380)+1.0) * 4.0};

  byte row, col, dep;

  // For each row
  for(row=0; row<SIDE; row++) {
    float row_f = float(row); // Optimization: Keep a floating point value of the row number, instead of recasting it repeatedly.

    // For each column
    for(col=0; col<SIDE; col++) {
      float col_f = float(col); // Optimization.

      // For each depth
      for(dep=0; dep<SIDE; dep++) {
	float dep_f = float(dep); // Optimization.

	// Calculate the distance between this LED, and p1.
	point dist1 = { col_f - p1.x, row_f - p1.y,  dep_f - p1.z }; // The vector from p1 to this LED.
	float distance1 = sqrt( dist1.x*dist1.x + dist1.y*dist1.y + dist1.z*dist1.z);

	// Calculate the distance between this LED, and p2.
	point dist2 = { col_f - p2.x, row_f - p2.y,  dep_f - p2.z}; // The vector from p2 to this LED.
	float distance2 = sqrt( dist2.x*dist2.x + dist2.y*dist2.y + dist2.z*dist2.z);

	// Calculate the distance between this LED, and p3.
	point dist3 = { col_f - p3.x, row_f - p3.y,  dep_f - p3.z}; // The vector from p3 to this LED.
	float distance3 = sqrt( dist3.x*dist3.x + dist3.y*dist3.y + dist3.z*dist3.z);

	// Warp the distance with a sin() function. As the distance value increases, the LEDs will get light,dark,light,dark,etc...
	// You can use a cos() for slightly different shading, or experiment with other functions.
	float color_1 = distance1; // range: 0.0...1.0
	float color_2 = distance2;
	float color_3 = distance3;
	float color_4 = (sin( distance1 * distance2 * colorStretch )) + 2.0 * 0.5;
	// Square the color_f value to weight it towards 0. The image will be darker and have higher contrast.
	color_1 *= color_1 * color_4;
	color_2 *= color_2 * color_4;
	color_3 *= color_3 * color_4;
	color_4 *= color_4;
	// Scale the color up to 0..7 . Max brightness is 7.
	//strip.setPixelColor(col + (8 * row), strip.Color(color_4, 0, 0) );
	plasmaColor.red=color_1*plasmaBrightness;
	plasmaColor.green=color_2*plasmaBrightness;
	plasmaColor.blue=color_3*plasmaBrightness;

	setPixel(row,col,dep,strip, &plasmaColor);       
      }
    }
  }
}
