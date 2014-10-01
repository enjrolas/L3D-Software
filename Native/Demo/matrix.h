#include "neopixel/neopixel.h"
#include "datatypes.h"
#include <math.h>
#define MATRIX_STRANDS 32

//function definitions
void newStrand(matrix* mat);
void updateStrand(matrix* mat);
void drawStrand(matrix* mat, Adafruit_NeoPixel& strip);
void initMatrix();
void updateMatrix(Adafruit_NeoPixel& strip);
