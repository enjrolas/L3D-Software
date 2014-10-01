#ifndef datatypes_h
#define datatypes_h

typedef struct{
  unsigned char red, green, blue;
} color;

typedef struct{
  unsigned char x,y;
  float z, zInc, zEnd;
} matrix;

typedef struct{
  float x;
  float y;
  float z;
} point;

#endif
