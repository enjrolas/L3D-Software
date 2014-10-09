#include "stdlib.h"
#include "util.h"

float frand(float min, float max) {
    float range = max - min;
    return min + range * (float)rand() / (float)RAND_MAX;
}
