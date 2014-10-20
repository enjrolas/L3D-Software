#include "application.h"
#include <stdarg.h>
#include <math.h>

#include "colors.h"
#include "cube.h"
#include "kiss_fftr.h"

bool tried_connecting = false;
SYSTEM_MODE(SEMI_AUTOMATIC);
#define INTERNET_SWITCH D2

#define PIN_X 14
#define PIN_Y 15
#define PIN_Z 13

#define MICROPHONE 12
#define GAIN_CONTROL 11
#define DIAL 10

int fft_size;
kiss_fftr_cfg fft_cfg;
kiss_fft_scalar *fft_in;
kiss_fft_cpx *fft_out;

unsigned int sampleMic() {
    analogWrite(GAIN_CONTROL, 45); //put the gain right in the middle, for now
    return analogRead(MICROPHONE);
}

void updateFFT() {
    kiss_fft_scalar pt;

    for(int i=0; i < fft_size; i++) {
        fft_in[i] = ((float)sampleMic())/4096.f;
    }

    kiss_fftr(fft_cfg, fft_in, fft_out);
}

void setup() {
    Serial.begin(9600);
    pinMode(INTERNET_SWITCH, INPUT);
    pinMode(GAIN_CONTROL, OUTPUT);

    fft_size = 128;
    fft_cfg = kiss_fftr_alloc(fft_size, FALSE, NULL, NULL);
    fft_in = (kiss_fft_scalar*)malloc(fft_size * sizeof(kiss_fft_scalar));
    fft_out = (kiss_fft_cpx*)malloc(fft_size / 2 * sizeof(kiss_fft_cpx) + 1);

    cube_init();
}

void loop() {
    // connect to cloud if internet switch is set
    if(digitalRead(INTERNET_SWITCH) && !tried_connecting) {
        Spark.connect();
        tried_connecting = true;
    }

    updateFFT();

    /*for(int z=0; z < 7; z++) {
        for(int x=0; x < 7; x++) {
            height[z][x] = height[z+1][x];
        }
    }*/

    /*for(int i=0; i < fft_size; i++) {
        Serial.print(log_pwr_fft[i]);
        Serial.print(",\t");
    }
    Serial.println();*/

    for(int x=0;x<8;x++) {
        for(int z=0;z<8;z++) {
            for(int y=0;y<8;y++) {
                setPixel(x,y,z, &color_dark);
            }
        }
    }

    color col = { 55, 55, 55 };
    int xoff = analogRead(DIAL) >> 4;
    int yoff = 2;

    for(int x=0;x<8;x++) {
        for(int z=0;z<8;z++) {
            int v = fft_out[(x + xoff)%(fft_size/2+1)].i + yoff;
            v = (v > 8)? 8: v;
            v = (v < 0)? 0: v;

            for(int y=0;y<v;y++) {
                setPixel(x,y,z, &col);
            }
        }
    }

    cube_update();
}
