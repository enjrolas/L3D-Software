#ifndef matrix_h
#define matrix_h

typedef struct{
  unsigned char red, green, blue;
} color;

typedef struct{
  unsigned char x,y;
  float z, zInc, zEnd;
} matrix;


#endif
