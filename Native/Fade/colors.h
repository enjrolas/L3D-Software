#ifndef colors_h
#define colors_h

typedef struct{
  unsigned char red, green, blue;
} color;

typedef struct{
  int x, y, z;
} point;

//function definitions
color lerpColor(color* a, color* b, int val, int min, int max);
color colorMap(float val, float min, float max);

#endif
