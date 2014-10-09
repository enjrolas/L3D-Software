#include "stdlib.h"
#include "math.h"
#include "util.h"

float frand(float min, float max) {
    float range = max - min;
    return min + range * (float)rand() / (float)RAND_MAX;
}

float dist(float x1, float y1, float z1, float x2, float y2, float z2) {
    return sqrt(pow(x2-x1, 2) + pow(y2-y1, 2) + pow(z2-z1, 2));
}
