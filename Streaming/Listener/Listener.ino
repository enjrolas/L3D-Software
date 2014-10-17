#include "neopixel/neopixel.h"

UDP Udp;
unsigned count = 0;

char data[512];

#define PIXEL_PIN D0
#define PIXEL_COUNT 512
#define PIXEL_TYPE WS2812B

#define SIDE 8

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
char localIP[24];
char macAddress[20];
int port;
int lastUpdated=0;

typedef struct{
    unsigned char red, green, blue;
} color;

void setPixel(int x, int y, int z, color col);  //sets a pixel at position (x,y,z) to the col parameter's color
int setPort(String _port);

void setup() 
{
    port=2000;
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Udp.begin (port);
  Serial.begin(115200);
  Serial.println("initializing...");
  updateNetworkInfo();
  initSparkVariables();
}

//initializes the shared variables and functions that are accessible through the spark API
//this makes the core's local IP address, MAC address and port accessible to streaming programs
//that have the access token for the core owner's account
//The function setPort lets a streaming program set the port on which the core will listen for streaming packets
void initSparkVariables()
{
    Spark.variable("IPAddress", localIP, STRING);
    Spark.variable("MACAddress", macAddress, STRING);
    Spark.variable("port", &port, INT);
    Spark.function("setPort", setPort);
}

//updates the local IP address and mac address and stores them in global strings
//if those strings are Spark variables (using the Spark.variable() function), they'll be accessible to the wider world
//This is all kinds of helpful anytime you're working on a project that uses network communicqtion (streaming, etc)

void updateNetworkInfo()
{
    IPAddress myIp = WiFi.localIP();
    sprintf(localIP, "%d.%d.%d.%d", myIp[0], myIp[1], myIp[2], myIp[3]);
    byte macAddr[6];
    WiFi.macAddress(macAddr);
    sprintf(macAddress, "%02x:%02x:%02x:%02x:%02x:%02x",macAddr[5],macAddr[4],macAddr[3],macAddr[2],macAddr[1],macAddr[0]);

    //print it out the serial port
    Serial.print("local IP Address: ");
    Serial.println(localIP);
    Serial.print("MAC address:  ");
    Serial.println(macAddress);
}

int setPort(String _port)
{
    port=_port.toInt();
    Udp.begin(port);
    return port;
}


void loop (void)
{
int32_t bytesrecv = Udp.parsePacket();

if(millis()-lastUpdated>60000)  //update the network settings every minute
{
    updateNetworkInfo();
    lastUpdated=millis();
}

if (bytesrecv==PIXEL_COUNT) {
    Udp.read(data,bytesrecv);
    for(int x=0;x<SIDE;x++)
        for(int y=0;y<SIDE;y++)
            for(int z=0;z<SIDE;z++)
            {
                int index = z*64 + y*8 + x;
//                color pixelColor={ data[index]&0xE0, (data[index]&0x1C)<<3, (data[index]&0x03)<<6};   //colors as encoded
//                color pixelColor={ (data[index]&0xE0)>>1, (data[index]&0x1C)<<2, (data[index]&0x03)<<5};   //colors with max brightness set to 128
                color pixelColor={ (data[index]&0xE0)>>2, (data[index]&0x1C)<<1, (data[index]&0x03)<<4};   //colors with max brightness set to 64
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

