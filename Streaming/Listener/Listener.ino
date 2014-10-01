#include "colors.h"
#include "neopixel/neopixel.h"

UDP Udp;
unsigned count = 0;

char data[512];

#define PIXEL_PIN D0
#define PIXEL_COUNT 512
#define PIXEL_TYPE WS2812B

#define SIDE 8

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);


void setup() 
{
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Udp.begin (6000);
  Serial.begin(115200);
  Serial.println("initializing...");
}

void loop (void)
{

int32_t retries = 0;
int32_t bytesrecv = Udp.parsePacket();
while(bytesrecv == 0 && retries < 1000) {
    bytesrecv = Udp.parsePacket();
    retries++;
}

if (bytesrecv==PIXEL_COUNT) {
    Udp.read(data,bytesrecv);
    for(int x=0;x<SIDE;x++)
        for(int y=0;y<SIDE;y++)
            for(int z=0;z<SIDE;z++)
            {
                int index = z*64 + y*8 + x;
                if(data[index]!=0)
                {
                    Serial.print("position:  (");
                    Serial.print(x);
                    Serial.print(", ");
                    Serial.print(y);
                    Serial.print(", ");
                    Serial.print(z);
                    Serial.println(")");
                }
                color pixelColor={ data[index]&0xE0, (data[index]&0x1C)<<3, (data[index]&0x03)<<6}; 
                setPixel(x,y,z,pixelColor);
            }
        
    }
    strip.show();
}

//sets a pixel at position (x,y,z) to the col parameter's color
void setPixel(int x, int y, int z, color col)
{
    int index = z*64 + x*8 + y;
    strip.setPixelColor(index,strip.Color(col.red,  col.green, col.blue));
}
