#include "colors.h"
#include "neopixel/neopixel.h"

#define PIXEL_PIN D0
#define PIXEL_COUNT 512
#define PIXEL_TYPE WS2812B
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

#define SIDE 8
//SYSTEM_MODE(SEMI_AUTOMATIC);  //don't connect to the internet on boot
#define BUTTON D2 //press this button to connect to the internet
int colorific;
bool fading=false;
int fadeValue=255;

void setup() {
    strip.begin();
    Spark.variable("colorific", &colorific, INT);
    //set the input mode for the 'connect to cloud' button
    pinMode(BUTTON, INPUT);
    
}

void loop() {
    //if the 'connect to cloud' button is pressed, try to connect to wifi.  
    //otherwise, run the program
    //note -- how does this behave when there are no wifi credentials loaded on the spark?
    if(!digitalRead(BUTTON))
        Spark.connect();
        
    if(!fading)
    {
        randomize();
        fading=true;
        fadeValue=255;
    }
    else
    {
        fadeValue--;
        fade();
        //if we're done fading)
        if(fadeValue==0)
            fading=false;
    }
        
    strip.show();
    if(!fading)
        delay(2000);  //wait for a few seconds to show off the pretty colors;

}

void randomize()
{
    color col;
    for(int x=0;x<SIDE;x++)
        for(int y=0;y<SIDE;y++)
            for(int z=0;z<SIDE;z++)
                {
                    col.red=rand()%255;
                    col.green=rand()%255;
                    col.blue=rand()%255;
                    /*
                    col.red=25;
                    col.green=50;
                    col.blue=100;
                    */
                    setPixel(x,y,z,&col);
                }    
}

void fade()
{
    color pixelColor;
        for(int x=0;x<SIDE;x++)
            for(int y=0;y<SIDE;y++)
                for(int z=0;z<SIDE;z++)
                    {
                        pixelColor=getPixel(x,y,z);
                        if(pixelColor.red>0)
                            pixelColor.red--;
                        if(pixelColor.green>0)
                            pixelColor.green--;
                        if(pixelColor.blue>0)
                            pixelColor.blue--;
                        setPixel(x,y,z,&pixelColor);    
                    }
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

//returns the color value currently displayed at the x,y,z location
color getPixel(int x, int y, int z)
{
    int index = (z*SIDE*SIDE) + (x*SIDE) + y;
    uint32_t col=strip.getPixelColor(index);
    color pixelColor;
    pixelColor.red=(col>>16)&255;
    pixelColor.green=(col>>8)&255;
    pixelColor.blue=col&255;
    return pixelColor;
}