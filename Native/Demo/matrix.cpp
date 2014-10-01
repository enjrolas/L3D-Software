#include <math.h>
#include <stdlib.h>
#include "matrix.h"
#include "cube.h"

matrix matrices[MATRIX_STRANDS];
color matrixTip, matrixStrand;

void initMatrix()
{
  //set up some colors that we'll use in the program
  matrixTip.red=50;
  matrixTip.green=160;
  matrixTip.blue=40;
    
  matrixStrand.red=14;
  matrixStrand.green=70;
  matrixStrand.blue=16;
  //initialize all the matrix 'strands'
  for(int i=0;i<MATRIX_STRANDS;i++)
    newStrand(&matrices[i]);
}


void newStrand(matrix* mat)
{
  mat->x=rand()%SIDE;
  mat->y=rand()%SIDE;
  mat->zInc=rand()%150;
  mat->zInc/=100;
  mat->zInc-=0.75;
  if(abs(mat->zInc)<0.01)
    {
      if(mat->zInc>0)
	mat->zInc+=0.01;
      else
	mat->zInc-=0.01;
    }
  if(mat->zInc<0)
    {
      mat->zEnd=-1*rand()%20;
      mat->z=rand()%20;
    }
  else
    {
      mat->zEnd=rand()%20;
      mat->z=-1*rand()%20;
    }
}

void updateStrand(matrix* mat)
{
  mat->z+=mat->zInc;
  if(abs(mat->z-mat->zEnd)<1)
    newStrand(mat);
}   

void drawStrand(matrix* mat, Adafruit_NeoPixel& strip)
{
  if(mat->zInc<1)
    {
      if(mat->z<SIDE)
        {
	  for(int i=SIDE-1;((i>mat->z)&&(i>=0));i--)
	    setPixel(mat->x, i, mat->y, strip, &matrixStrand);
	  if(mat->z>=0)
	    setPixel(mat->x, mat->z, mat->y, strip, &matrixTip);
        }
    }
  else
    {
      if(mat->z<SIDE)
        {
	  for(int i=0;((i<mat->z)&&(i<SIDE));i++)
	    setPixel(mat->x, i, mat->y, strip, &matrixStrand);
	  if(mat->z>=0)
	    setPixel(mat->x, mat->z, mat->y, strip, &matrixTip);
        }
    }
}

void updateMatrix(Adafruit_NeoPixel& strip)
{
  //update and draw all the matrix 'strands'
  for(int i=0;i<MATRIX_STRANDS;i++)
    {
      drawStrand(&matrices[i], strip);
      updateStrand(&matrices[i]);
    }
}
