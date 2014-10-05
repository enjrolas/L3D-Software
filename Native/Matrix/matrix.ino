#include "matrix.h"
#include "neopixel/neopixel.h"

#define MATRIX_STRANDS 32
#define SIDE 8
#include <math.h>

#define PIXEL_PIN D0
#define PIXEL_COUNT 512
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

SYSTEM_MODE(SEMI_AUTOMATIC);  //don't connect to the internet on boot
#define BUTTON D2 //press this button to connect to the internet

//sets a pixel at position (x,y,z) to the col parameter's color
void setPixel(int x, int y, int z, color* col);
//function definitions
void newMatrix(matrix* mat);
void updateMatrix(matrix* mat);
void drawMatrix(matrix* mat);

matrix matrices[MATRIX_STRANDS];
color black, matrixTip, matrixStrand;


bool onlinePressed=false;
bool lastOnline=true;

double num;
void setup() 
{
 
 Spark.variable("num", &num, DOUBLE);
// seed the random number generator.  THINGS WILL NEVER BE THE SAME AGAIN
    uint32_t seed = millis(); 
  srand(seed); 
  
  //set up some colors that we'll use in the program
    black.red=0;
    black.green=0;
    black.blue=0;

    matrixTip.red=50;
    matrixTip.green=160;
    matrixTip.blue=40;
    
    matrixStrand.red=14;
    matrixStrand.green=70;
    matrixStrand.blue=16;
    

    initCloudButton();

    //initialize all the matrix 'strands'
    for(int i=0;i<MATRIX_STRANDS;i++)
        newMatrix(&matrices[i]);
}

//sets up the online/offline switch
void initCloudButton()
{
  //set the input mode for the 'connect to cloud' button
  pinMode(BUTTON, INPUT_PULLUP);
    //a.k.a. onlinePressed is HIGH when the switch is set to 'online' and LOW when the switch is set to 'offline'
    onlinePressed=digitalRead(BUTTON);
    if(onlinePressed)
        Spark.connect();
}

//checks to see if the 'online/offline' switch is switched
void checkCloudButton()
{
    //if the 'connect to cloud' button is pressed, try to connect to wifi.  
    //otherwise, run the program
    //note -- how does this behave when there are no wifi credentials loaded on the spark?

    //onlinePressed is HIGH when the switch is _not_ connected and LOW when the switch is connected
    //a.k.a. onlinePressed is HIGH when the switch is set to 'online' and LOW when the switch is set to 'offline'
    onlinePressed=digitalRead(BUTTON);
    
    if((!onlinePressed)&&(lastOnline))  //marked as 'offline'
    {
        lastOnline=onlinePressed;
        Spark.disconnect();
    }    

    else if((onlinePressed)&&(!lastOnline))  //marked as 'online'
    {
        lastOnline=onlinePressed;
        Spark.connect();
    }

    lastOnline=onlinePressed;
}

void loop() 
{
    checkCloudButton();
        
    for(int x=0;x<SIDE;x++)
        for(int y=0;y<SIDE;y++)
            for(int z=0;z<SIDE;z++)
                setPixel(x,y,z, &black);


    //initialize all the matrix 'strands'
    for(int i=0;i<MATRIX_STRANDS;i++)
    {
        drawMatrix(&matrices[i]);
        updateMatrix(&matrices[i]);
    }
    

    strip.show();
}


//sets a pixel at position (x,y,z) to the col parameter's color
void setPixel(int x, int y, int z, color* col)
{
    int index = (z*SIDE*SIDE) + (x*SIDE) + y;
    int rando=10;
    if(col->green>rando/2)
        strip.setPixelColor(index,strip.Color(col->red,  rand()%rando - rando/2 + col->green, col->blue));
    else
        strip.setPixelColor(index,strip.Color(col->red,  col->green, col->blue));
    
}


void newMatrix(matrix* mat)
{
    mat->x=rand()%SIDE;
    mat->y=rand()%SIDE;
    mat->zInc=rand()%150;
    mat->zInc/=100;
    mat->zInc-=0.75;
    num=mat->zInc;
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

void updateMatrix(matrix* mat)
{
    mat->z+=mat->zInc;
    if(abs(mat->z-mat->zEnd)<1)
        newMatrix(mat);
}   

void drawMatrix(matrix* mat)
{
    if(mat->zInc<1)
    {
        if(mat->z<SIDE)
        {
            for(int i=SIDE-1;((i>mat->z)&&(i>=0));i--)
                setPixel(mat->x, i, mat->y,&matrixStrand);
            if(mat->z>=0)
                setPixel(mat->x, mat->z, mat->y, &matrixTip);
        }
    }
    else
    {
        if(mat->z<SIDE)
        {
            for(int i=0;((i<mat->z)&&(i<SIDE));i++)
                setPixel(mat->x, i, mat->y,&matrixStrand);
            if(mat->z>=0)
                setPixel(mat->x, mat->z, mat->y, &matrixTip);
        }
    }
}

