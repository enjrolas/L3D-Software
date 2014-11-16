
/**************************************

Audio by Owen Trueblood
October 2014

 *************************************/

#include "application.h"
#include <stdarg.h>
#include <math.h>

#include "colors.h"
#include "cube.h"
#include "kiss_fftr.h"

bool tried_connecting = false;
SYSTEM_MODE(SEMI_AUTOMATIC);  //don't connect to the internet on boot
#define BUTTON D2 //press this button to connect to the internet
#define MODE D3
bool onlinePressed=false;
bool lastOnline=true;

#define PIN_X 14
#define PIN_Y 15
#define PIN_Z 13

#define MICROPHONE 12
#define GAIN_CONTROL 11
#define DIAL 10

#define FFT_SIZE 256
kiss_fftr_cfg fft_cfg;
kiss_fft_scalar *fft_in;
kiss_fft_cpx *fft_out;

uint8_t height[8][8];

void updateFFT() {
    kiss_fft_scalar pt;

    for(int i=0; i < FFT_SIZE; i++) {
        fft_in[i] = ((float)analogRead(MICROPHONE))/4096.f;
    }

    kiss_fftr(fft_cfg, fft_in, fft_out);
}
void initCloudButton();
void checkCloudButton();

void setup() {
    Serial.begin(9600);
    initCloudButton();
    pinMode(GAIN_CONTROL, OUTPUT);
    analogWrite(GAIN_CONTROL, 45); //put the gain right in the middle, for now

    fft_cfg = kiss_fftr_alloc(FFT_SIZE, FALSE, NULL, NULL);
    fft_in = (kiss_fft_scalar*)malloc(FFT_SIZE * sizeof(kiss_fft_scalar));
    fft_out = (kiss_fft_cpx*)malloc(FFT_SIZE / 2 * sizeof(kiss_fft_cpx) + 1);

    cube_init();
}

//sets up the online/offline switch
void initCloudButton()
{
  //set the input mode for the 'connect to cloud' button
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(MODE, INPUT_PULLUP);
  if(!digitalRead(MODE))
    WiFi.listen();
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
    
  if(!digitalRead(MODE))
    WiFi.listen();
}


void palettize(color* col, int x) {
    /*
    col->red = 256*sin(M_PI * x / 32);
    col->green = 256*sin(M_PI * x / 64);
    col->blue = 256*sin(M_PI * x / 128);
    */
    
    col->red = 256*sin(M_PI * x / 128);
    col->green = 256*sin(M_PI * x / 128);
    col->blue = 0;
}

void loop() {
  checkCloudButton();

    updateFFT();

    /*for(int i=0; i < FFT_SIZE; i++) {
        Serial.print(log_pwr_fft[i]);
        Serial.print(",\t");
    }
    Serial.println();*/

    color col = { 55, 55, 55 };

    // shift the previous slices
    for(int y=7; y >= 1; y--) {
        for(int x=0; x < 8; x++) {
            for(int z=0; z < 8; z++) {
                color below = getPixel(x, y-1, z);
                setPixel(x, y, z, &below);
            }
        }
    }

    // add the next slice
    uint8_t x = 4;
    uint8_t z = 4;
    uint8_t magic[4] = {0, 1, 0, -1};
    bool done = false;
    int count = 0;
    for(int i=1; count < 64; i++) {
        for(int j=0; j<2 && count < 64; j++) {
            for(int k=0; k<i && count < 64; k++) {
                int index = i*2+j;
                x += magic[index%4];
                z += magic[(index+1)%4];

                int xoff = 0;
                int yoff = 0;

                float h = 0;
                for(int s=0; s < FFT_SIZE/64; s++) {
                    h += fft_out[(s + (count + xoff)*FFT_SIZE/64)%FFT_SIZE].i + yoff;
                }
                h /= FFT_SIZE/64;

                palettize(&col, h*32);
                setPixel(x, 0, z, &col);

                count++;
            }
        }
    }

    cube_update();
}
