#include "neopixel/neopixel.h"
#include "zPlasma.h"
#include "cube.h"
#include "fireworks.h"
#include "squarral.h"
#include <math.h>
#include "matrix.h"
#include "datatypes.h"

SYSTEM_MODE(SEMI_AUTOMATIC);  //don't connect to the internet on boot
#define BUTTON D2 //press this button to connect to the internet

#define MATRIX 0
#define FIREWORKS 1
#define PLASMA 2
#define SQUARRAL 3
#define DEMO_ROUTINES 4

bool onlinePressed=false;
bool lastOnline=true;
int demo=SQUARRAL;

Adafruit_NeoPixel strip=Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

int frameCount=0;
int runTime;

bool fading=false;
int fadeValue=255;

void setup() {
    Spark.variable("frameCount", &frameCount, INT);
    Spark.variable("runTime", &runTime, INT);
    Spark.variable("demo", &demo, INT);
 // seed the random number generator.  THINGS WILL NEVER BE THE SAME AGAIN
 uint32_t seed = millis(); 
 srand(seed); 
 initCube();
  //set the input mode for the 'connect to cloud' button
  pinMode(BUTTON, INPUT_PULLDOWN);
    //a.k.a. onlinePressed is HIGH when the switch is set to 'online' and LOW when the switch is set to 'offline'
    onlinePressed=digitalRead(BUTTON);
    if(onlinePressed)
        Spark.connect();
        
  initMatrix();
  initFireworks();
  initSquarral();
  computeRunTime();  
}

void computeRunTime()
{
    runTime=1000+(rand()%1000)-500;
//    if(demo==SQUARRAL)
//        runTime*=2;
    if(demo==PLASMA)  //plasma is more computationally intensive, so the frames take longer to render.  
        runTime*=3/4;
}

void loop() {
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
    
    
    if(fading)
        fade();
    else
    {
        blackBackground(strip);
        switch(demo)
        {
            case(MATRIX):
                updateMatrix(strip);
            break;
            case(FIREWORKS):
                updateFireworks(strip);
            break;
            case(PLASMA):
                zPlasma(strip);
            break;
            case(SQUARRAL):
                squarral(strip);
            break;
        }
        frameCount++;
    }
    strip.show();
    if(frameCount>=runTime)
    {
        computeRunTime();
        frameCount=0;
        demo=rand()%DEMO_ROUTINES;
        fading=true;  
    }
    if(fading)
    {
        fadeValue--;
        fade();
        //if we're done fading)
        if(fadeValue==0)
        {
            fading=false;
            fadeValue=255;
        }
    }
}

void fade()
{
    color pixelColor;
        for(int x=0;x<SIDE;x++)
            for(int y=0;y<SIDE;y++)
                for(int z=0;z<SIDE;z++)
                    {
                        pixelColor=getPixel(x,y,z, strip);
                        if(pixelColor.red>0)
                            pixelColor.red--;
                        if(pixelColor.green>0)
                            pixelColor.green--;
                        if(pixelColor.blue>0)
                            pixelColor.blue--;
                        setPixel(x,y,z, strip, &pixelColor);    
                    }
}
