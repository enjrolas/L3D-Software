#include "neopixel/neopixel.h"
#include <math.h>
//set up the pin that controls the LEDs, the type of LEDs (WS2812B) and the number of LEDs in the cube (8*8*8=512)
#define PIXEL_PIN D0
#define PIXEL_COUNT 512
#define PIXEL_TYPE WS2812B
#define SIDE 8

SYSTEM_MODE(SEMI_AUTOMATIC);  //don't connect to the internet on boot
#define BUTTON D2 //press this button to connect to the internet
#define MODE D3
#define MICROPHONE 12
#define GAIN_CONTROL 11
#define MAX_POINTS 20
#define SPEED 0.22
#define MIN_SALVO_SPACING 100

bool onlinePressed=false;
bool lastOnline=true;

#define FIREWORKS 0
#define PLASMA 1
#define SQUARRAL 2
#define PURPLE_RAIN 3
#define DEMO_ROUTINES 4

/*  datatype definitions
*/

typedef struct{
    unsigned char red, green, blue;
} color;

typedef struct{
float x;
float y;
float z;
} point;

typedef struct{
    point raindrops[MAX_POINTS];
    bool dead;
} salvo;

/******************************
 * function definitions
 * ***************************/
 void background(color col);
 color getPixel(int x, int y, int z);
void setPixel(int x, int y, int z, color col);
color colorMap(float val, float min, float max);
color lerpColor(color a, color b, int val, int min, int max);
void add(point& a, point& b);


/******************************
 * fireworks variables *
 * ****************************/
 color black;
 int centerX, centerY, centerZ;
int launchX, launchZ;
int red, green, blue;
int brightness=35;
float radius=0;
float speed;
bool showRocket;
bool exploded;
float xInc, yInc, zInc;
float rocketX, rocketY, rocketZ;
float launchTime;
int maxSize;
color rocketColor, fireworkColor;



/*********************************
 * squarral variables *
 * ******************************/

#define TRAIL_LENGTH 50

int frame=0;
color pixelColor;
point position, increment, pixel;
point trailPoints[TRAIL_LENGTH];
int posX, posY, posZ;
int incX, incY, incZ;
int squarral_zInc=1;
int bound=0;
int boundInc=1;
unsigned char axis=0;
bool rainbow=true;

//maxBrightness is the brightness limit for each pixel.  All color data will be scaled down 
//so that the largest value is maxBrightness
int maxBrightness=50;

/********************************
 * zplasma variables *
 * *****************************/
float phase = 0.0;
float phaseIncrement = 0.035; // Controls the speed of the moving points. Higher == faster
float colorStretch = 0.23; // Higher numbers will produce tighter color bands 
float plasmaBrightness = 0.2;
color plasmaColor;

/*********************************
 * purple rain variables *
 * *******************************/
 int threshhold;
int max;
int min;
float sensitivity=0.5;
int maxAmplitude=0;
bool aboveThreshhold=false;
int timeAboveThreshhold;
color rainColor;
salvo salvos[SIDE];

/**********************************
 * flip variables *
 * ********************************/
 //accelerometer pinout
#define X 13
#define Y 14
#define Z 15
#define AUTOCYCLE_TIME 15000
#define FACEPLANT 2300
#define UPSIDE_DOWN 1850
#define RIGHTSIDE_UP 2400
#define LEFT_SIDE 1800
#define RIGHT_SIDE 2400
#define FLIP_TIMEOUT 3000
#define FLIP_DEBOUNCE 250

long lastFaceplant=-1*FLIP_TIMEOUT;
bool upsideDown=false;
bool sideways=false;
bool autoCycle=true;    //start on autocycle by default
int upsideDownTime=-1*FLIP_TIMEOUT;
long lastAutoCycle=0;
int lastLeft=-1*FLIP_TIMEOUT;
int lastRight=-1*FLIP_TIMEOUT;
int accelerometer[3];
long lastChange=0;



int demo=FIREWORKS;

Adafruit_NeoPixel strip=Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

int frameCount=0;

/*******************************
 * fade variables *
 * ****************************/
bool fading=false;
int fadeValue=255;
int fadeSpeed=2;


void setup() {
    pinMode(7,OUTPUT);
    digitalWrite(7, HIGH);
 // seed the random number generator.  THINGS WILL NEVER BE THE SAME AGAIN
 uint32_t seed = millis(); 
 srand(seed); 
// Serial.begin(115200);
 initCube();
 initCloudButton();
 initSquarral();
 initFireworks();
 initMicrophone();
 initSalvos();

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
    
    if((!onlinePressed)&&(lastOnline))  //marked as 'online'
    {
        lastOnline=onlinePressed;
        Spark.connect();
    }    

    else if((onlinePressed)&&(!lastOnline))  //marked as 'offline'
    {
        lastOnline=onlinePressed;
        Spark.disconnect();
    }

    lastOnline=onlinePressed;
    
    if(!digitalRead(MODE))
        WiFi.listen();
}

void loop() {
    //if the 'connect to cloud' button is pressed, try to connect to wifi.  
    //otherwise, run the program
    checkCloudButton();
    
    
    if(fading)
        fade();
    else
    {
        background(black);
        switch(demo)
        {
            case(FIREWORKS):
                updateFireworks();
            break;
            case(PLASMA):
                zPlasma();
            break;
            case(SQUARRAL):
                squarral();
            break;
            case(PURPLE_RAIN):
                purpleRain();
            break;
            
        }
        frameCount++;
    }
    //check to see how if the cube has been flipped
    checkFlipState();

    strip.show();
    
    if(fading)
    {
        fadeValue-=fadeSpeed;
        //if we're done fading)
        if(fadeValue<=0)
        {
            fading=false;
            fadeValue=255;
        }
        else
            fade();
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
                        setPixel(x,y,z, pixelColor);    
                    }
}


//sets a pixel at position (x,y,z) to the col parameter's color
void setPixel(int x, int y, int z, color col)
{
    int index = (z*SIDE*SIDE) + (x*SIDE) + y;
        strip.setPixelColor(index,strip.Color(col.red,  col.green, col.blue));
    
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

void initCube()
{
    black.red=0;
    black.green=0;
    black.blue=0;
}


void background(color col)
{
    for(int x=0;x<SIDE;x++)
        for(int y=0;y<SIDE;y++)
            for(int z=0;z<SIDE;z++)
                setPixel(x,y,z, col);    
}


/***************************************
 * fireworks functions *
 * ***********************************/
 
 
void updateFireworks()
{
//loop through all the pixels, calculate the distance to the center point, and turn the pixel on if it's at the right radius
  for(int x=0;x<SIDE;x++)
    for(int y=0;y<SIDE;y++) 
        for(int z=0;z<SIDE;z++)
        {
            if(showRocket)
                if(abs(distance(x,y,z,rocketX, rocketY, rocketZ)-radius)<0.05)
                     setPixel(x,y,z, rocketColor);                
            if(exploded)
                if(abs(distance(x,y,z,centerX, centerY, centerZ)-radius)<0.1)
                  setPixel(x,y,z, fireworkColor);
        }

        if(exploded)
            radius+=speed;  //the sphere gets bigger
        if(showRocket)
        {
            rocketX+=xInc;
            rocketY+=yInc;
            rocketZ+=zInc;
        }
        //if our sphere gets too large, restart the animation in another random spot
        if(radius>maxSize)
            prepRocket();
        if(abs(distance(centerX,centerY,centerZ,rocketX, rocketY, rocketZ)-radius)<2)
            {
                showRocket=false;
                exploded=true;
            }
        
}

float distance(float x, float y, float z, float x1, float y1, float z1)
{
    return(sqrt(pow(x-x1,2)+pow(y-y1,2)+pow(z-z1,2)));
}

void prepRocket()
{
            radius=0;
            centerX=rand()%8;
            centerY=rand()%8;
            centerZ=rand()%8;
            fireworkColor.red=rand()%brightness;
            fireworkColor.green=rand()%brightness;
            fireworkColor.blue=rand()%brightness;
            launchX=rand()%8;
            launchZ=rand()%8;
            rocketX=launchX;
            rocketY=0;
            rocketZ=launchZ;
            launchTime=15+rand()%25;
            xInc=(centerX-rocketX)/launchTime;
            yInc=(centerY-rocketY)/launchTime;
            zInc=(centerZ-rocketZ)/launchTime;
            showRocket=true;
            exploded=false;
            speed=0.15;
            maxSize=2+rand()%6;
            //speed=rand()%5;
            //speed*=0.1;
}

void initFireworks()
{
    rocketColor.red=255;
    rocketColor.green=150;
    rocketColor.blue=100;
    prepRocket();
}

void initSquarral() 
{
  position={0,0,0};
  increment={1,0,0};
}

void squarral() 
{
    add(position, increment);
    if((increment.x==1)&&(position.x==SIDE-1-bound))
        increment={0,1,0};
    if((increment.x==-1)&&(position.x==bound))
        increment={0,-1,0};
    if((increment.y==1)&&(position.y==SIDE-1-bound))
        increment={-1,0,0};
    if((increment.y==-1)&&(position.y==bound))
    {
        increment={1,0,0};
        position.z+=squarral_zInc;
        bound+=boundInc;
        if((position.z==3)&&(squarral_zInc>0))
          boundInc=0;
        if((position.z==4)&&(squarral_zInc>0))
          boundInc=-1;
        if((position.z==3)&&(squarral_zInc<0))
          boundInc=-1;
        if((position.z==4)&&(squarral_zInc<0))
          boundInc=0;
        
        if((position.z==0)||(position.z==SIDE-1))
            boundInc*=-1;
            
        if((position.z==SIDE-1)||(position.z==0))
        {
            squarral_zInc*=-1;
            if(squarral_zInc==1)
            {
                axis=rand()%6;
                if(rand()%5==0)
                    rainbow=true;
                else
                    rainbow=false;
            }
        }
    }
    
    posX=position.x;
    posY=position.y;
    posZ=position.z;
    
    incX=increment.x;
    incY=increment.y;
    incZ=increment.z;
    
    for(int i=TRAIL_LENGTH-1;i>0;i--)
    {
        trailPoints[i].x=trailPoints[i-1].x;
        trailPoints[i].y=trailPoints[i-1].y;
        trailPoints[i].z=trailPoints[i-1].z;
    }
    trailPoints[0].x=pixel.x;
    trailPoints[0].y=pixel.y;
    trailPoints[0].z=pixel.z;
    switch(axis){
        case(0):
            pixel.x=position.x;
            pixel.y=position.y;
            pixel.z=position.z;
            break;
        case(1):
            pixel.x=position.z;
            pixel.y=position.x;
            pixel.z=position.y;
            break;
        case(2):
            pixel.x=position.y;
            pixel.y=position.z;
            pixel.z=position.x;
            break;
        case(3):
            pixel.x=position.z;
            pixel.y=SIDE-1-position.x;
            pixel.z=position.y;
            break;
        case(4):
            pixel.x=position.y;
            pixel.y=position.z;
            pixel.z=SIDE-1-position.x;
            break;
        case(5):
            pixel.x=position.x;
            pixel.y=SIDE-1-position.y;
            pixel.z=position.z;
            break;
    }
        
    pixelColor=colorMap(frame%1000,0,1000);
    setPixel((int)pixel.x, (int)pixel.y, (int)pixel.z, pixelColor);
    for(int i=0;i<TRAIL_LENGTH;i++)
    {
        color trailColor;
        if(rainbow)
        {
            trailColor=colorMap((frame+(i*1000/TRAIL_LENGTH))%1000,0,1000);
            //fade the trail to black over the length of the trail
            trailColor.red=trailColor.red*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
            trailColor.green=trailColor.green*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
            trailColor.blue=trailColor.blue*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
        }
        else
        {
            trailColor.red=pixelColor.red*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
            trailColor.green=pixelColor.green*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
            trailColor.blue=pixelColor.blue*(TRAIL_LENGTH-i)/TRAIL_LENGTH;
        }
        setPixel((int)trailPoints[i].x, (int)trailPoints[i].y, (int)trailPoints[i].z, trailColor);
    }
    frame++;
}

void add(point& a, point& b)
{
    a.x+=b.x;
    a.y+=b.y;
    a.z+=b.z;
}


//returns a color from a set of colors fading from blue to green to red and back again
//the color is returned based on where the parameter *val* falls between the parameters
//*min* and *max*.  If *val* is min, the function returns a blue color.  If *val* is halfway
//between *min* and *max*, the function returns a yellow color.  
color colorMap(float val, float min, float max)
{
  float range=1024;
  val=range*(val-min)/(max-min);
  color colors[6];
  colors[0].red=0;
  colors[0].green=0;
  colors[0].blue=maxBrightness;
  
  colors[1].red=0;
  colors[1].green=maxBrightness;
  colors[1].blue=maxBrightness;
  
  colors[2].red=0;
  colors[2].green=maxBrightness;
  colors[2].blue=0;
  
  colors[3].red=maxBrightness;
  colors[3].green=maxBrightness;
  colors[3].blue=0;
  
  colors[4].red=maxBrightness;
  colors[4].green=0;
  colors[4].blue=0;

  colors[5].red=maxBrightness;
  colors[5].green=0;
  colors[5].blue=maxBrightness;
  
  if (val<=range/6)
    return(lerpColor(colors[0], colors[1], val, 0, range/6));
  else if (val<=2*range/6)
    return(lerpColor(colors[1], colors[2], val, range/6, 2*range/6));
  else if (val<=3*range/6)
    return(lerpColor(colors[2], colors[3], val, 2*range/6, 3*range/6));
  else if (val<=4*range/6)
    return(lerpColor(colors[3], colors[4], val, 3*range/6, 4*range/6));
  else if (val<=5*range/6)
    return(lerpColor(colors[4], colors[5], val, 4*range/6, 5*range/6));
  else
    return(lerpColor(colors[5], colors[0], val, 5*range/6, range));
}


//returns a color that's an interpolation between colors a and b.  The color
//is controlled by the position of val relative to min and max -- if val is equal to min,
//the resulting color is identical to color a.  If it's equal to max, the resulting color 
//is identical to color b.  If val is (max-min)/2, the resulting color is the average of
//color a and color b
color lerpColor(color a, color b, int val, int min, int max)
{
    color lerped;
    lerped.red=a.red+(b.red-a.red)*(val-min)/(max-min);
    lerped.green=a.green+(b.green-a.green)*(val-min)/(max-min);
    lerped.blue=a.blue+(b.blue-a.blue)*(val-min)/(max-min);
    return lerped;
}


/********************************
 * zplasma functions *
 * *****************************/
 
 void zPlasma()
{
  
phase += phaseIncrement;
// The two points move along Lissajious curves, see: http://en.wikipedia.org/wiki/Lissajous_curve
// We want values that fit the LED grid: x values between 0..8, y values between 0..8, z values between 0...8
// The sin() function returns values in the range of -1.0..1.0, so scale these to our desired ranges.
// The phase value is multiplied by various constants; I chose these semi-randomly, to produce a nice motion.
point p1 = { (sin(phase*1.000)+1.0) * 4, (sin(phase*1.310)+1.0) * 4.0,  (sin(phase*1.380)+1.0) * 4.0};
point p2 = { (sin(phase*1.770)+1.0) * 4, (sin(phase*2.865)+1.0) * 4.0,  (sin(phase*1.410)+1.0) * 4.0};
point p3 = { (sin(phase*0.250)+1.0) * 4, (sin(phase*0.750)+1.0) * 4.0,  (sin(phase*0.380)+1.0) * 4.0};

byte row, col, dep;

// For each row
for(row=0; row<SIDE; row++) {
float row_f = float(row); // Optimization: Keep a floating point value of the row number, instead of recasting it repeatedly.

// For each column
for(col=0; col<SIDE; col++) {
float col_f = float(col); // Optimization.

// For each depth
for(dep=0; dep<SIDE; dep++) {
float dep_f = float(dep); // Optimization.

// Calculate the distance between this LED, and p1.
point dist1 = { col_f - p1.x, row_f - p1.y,  dep_f - p1.z }; // The vector from p1 to this LED.
float distance1 = sqrt( dist1.x*dist1.x + dist1.y*dist1.y + dist1.z*dist1.z);

// Calculate the distance between this LED, and p2.
point dist2 = { col_f - p2.x, row_f - p2.y,  dep_f - p2.z}; // The vector from p2 to this LED.
float distance2 = sqrt( dist2.x*dist2.x + dist2.y*dist2.y + dist2.z*dist2.z);

// Calculate the distance between this LED, and p3.
point dist3 = { col_f - p3.x, row_f - p3.y,  dep_f - p3.z}; // The vector from p3 to this LED.
float distance3 = sqrt( dist3.x*dist3.x + dist3.y*dist3.y + dist3.z*dist3.z);

// Warp the distance with a sin() function. As the distance value increases, the LEDs will get light,dark,light,dark,etc...
// You can use a cos() for slightly different shading, or experiment with other functions.
float color_1 = distance1; // range: 0.0...1.0
float color_2 = distance2;
float color_3 = distance3;
float color_4 = (sin( distance1 * distance2 * colorStretch )) + 2.0 * 0.5;
// Square the color_f value to weight it towards 0. The image will be darker and have higher contrast.
color_1 *= color_1 * color_4;
color_2 *= color_2 * color_4;
color_3 *= color_3 * color_4;
color_4 *= color_4;
// Scale the color up to 0..7 . Max brightness is 7.
//strip.setPixelColor(col + (8 * row), strip.Color(color_4, 0, 0) );
plasmaColor.red=color_1*plasmaBrightness;
plasmaColor.green=color_2*plasmaBrightness;
plasmaColor.blue=color_3*plasmaBrightness;

setPixel(row,col,dep,plasmaColor);       
}
}
}
}

/*********************************************
 * purple rain functions *
 * *******************************************/
 
 void checkMicrophone()
{
    int mic=analogRead(MICROPHONE);
    if(mic<min)
        min=mic;
    if(mic>max)
        max=mic;
    float range=max-min;
    int mean=(max-min)/2;
    /*
    if(min<mean)
        min++;
    if(max>mean)
        max--;
        */
    threshhold=mean+sensitivity*(range/2);

    if(mic>threshhold)
    {
        if((!aboveThreshhold)&&((timeAboveThreshhold-millis())>MIN_SALVO_SPACING))
        {
            launchRain(mic-threshhold);
            aboveThreshhold=true;
            timeAboveThreshhold=millis();
        }
    }
    else
        aboveThreshhold=false;
/*
    Serial.print(mic);
    Serial.print(":  ");
    Serial.print(threshhold);
    Serial.print(" - above threshhold: ");
    Serial.println(aboveThreshhold);
    */
}

void launchRain(int amplitude)
{
    int i;
    for(i=0;((i<SIDE)&&(!salvos[i].dead));i++)
        ;
    if(i<SIDE)
    {
        if(amplitude>maxAmplitude)
            maxAmplitude=amplitude;
        int numDrops=map(amplitude,0, maxAmplitude,0, MAX_POINTS);
        for(int j=0;j<numDrops;j++)
        {
            salvos[i].raindrops[j].x=1+(rand()%6);
            salvos[i].raindrops[j].y=((rand()%10)-5)/10;
            salvos[i].raindrops[j].z=1+(rand()%6);
            salvos[i].dead=false;
        }
        for(int j=numDrops;j<MAX_POINTS;j++)
        {
            salvos[i].raindrops[j].x=-1;
            salvos[i].raindrops[j].z=-1;
        }

    }
    
}


void drawSalvos()
{
    for(int i=0;i<SIDE;i++)
        if(!salvos[i].dead)
            for(int j=0;j<MAX_POINTS;j++)
                setPixel(salvos[i].raindrops[j].x, salvos[i].raindrops[j].y, salvos[i].raindrops[j].z, rainColor);
}

void updateSalvos()
{
    for(int i=0;i<SIDE;i++)
    {
        int offCube=true;
        for(int j=0;j<MAX_POINTS;j++)
        {
            salvos[i].raindrops[j].y+=SPEED;
            if(salvos[i].raindrops[j].y<SIDE)
                offCube=false;
            else
            {
                salvos[i].raindrops[j].x=-1;
                salvos[i].raindrops[j].z=-1;
            }
        }
        if(offCube)
            salvos[i].dead=true;
    }
}

void initMicrophone()
{
  pinMode(GAIN_CONTROL, OUTPUT);
  digitalWrite(GAIN_CONTROL, LOW);
}

void initSalvos()
{
    for(int i=0;i<SIDE;i++)
    {
        for(int j=0;j<MAX_POINTS;j++)
        {
            salvos[i].raindrops[j].x=-1;
            salvos[i].raindrops[j].z=-1;
        }
        salvos[i].dead=true;
    }
  rainColor.red=100;
  rainColor.green=0;
  rainColor.blue=80;
}

void purpleRain()
{
    checkMicrophone();
    updateSalvos();
    drawSalvos();    
}

/****************************************
 * flip functions *
 * **************************************/
 
 void checkFlipState()
 {
    updateAccelerometer();
    /*
    if(accelerometer[2]<UPSIDE_DOWN)  //if the cube is upside-down, set the upside-down flag and mark the time when it was flipped
    {
        upsideDownTime=millis();
  //      Serial.println("I'm upside-down!");
    }
    */
    if(accelerometer[0]>FACEPLANT)  //if the cube is upside-down, set the upside-down flag and mark the time when it was flipped
    {
        lastFaceplant=millis();
  //      Serial.println("I'm upside-down!");
    }
    if(accelerometer[1]<LEFT_SIDE)  //if the cube is flipped to either side
    {
        lastLeft=millis();
//        Serial.println("I'm on my left side");
    }
    if(accelerometer[1]>RIGHT_SIDE)
    {
        lastRight=millis();
  //      Serial.println("I'm on my right side");
    }

    if(accelerometer[2]>RIGHTSIDE_UP)
    {
     //   Serial.println("whew!  I'm rightside-up");
     /*
        if(((millis()-upsideDownTime)<FLIP_TIMEOUT)&&(millis()-lastChange>FLIP_DEBOUNCE))
        {
     //       Serial.println("turned upside down and back");
            lastChange=millis();
            autoCycle=!autoCycle;
            upsideDownTime=millis()-FLIP_TIMEOUT;
            lastLeft=millis()-FLIP_TIMEOUT;  //clears the left and right turns, in case the user turned it sideways
            lastRight=millis()-FLIP_TIMEOUT;  //clears the left and right turns, in case the user turned it sideways
        }
        if(((millis()-lastLeft)<FLIP_TIMEOUT)&&(millis()-lastChange>FLIP_DEBOUNCE))
        {
    //        Serial.println("turned to the left and back");
            lastChange=millis();
            decrementDemo();
            lastLeft=millis()-FLIP_TIMEOUT;
        }
        if(((millis()-lastRight)<FLIP_TIMEOUT)&&(millis()-lastChange>FLIP_DEBOUNCE))
        {
       //     Serial.println("turned to the right and back");
            lastChange=millis();
            incrementDemo();
            lastRight=millis()-FLIP_TIMEOUT;
        }
    }
    */
        if(((millis()-lastFaceplant)<FLIP_TIMEOUT)&&(millis()-lastFaceplant>FLIP_DEBOUNCE))
        {
            autoCycle=false;
            lastFaceplant=millis()-FLIP_TIMEOUT;
            color flash;
            flash.red=maxBrightness;
            flash.green=maxBrightness;
            flash.blue=maxBrightness;
            background(flash);
        }
        if(((millis()-lastLeft)<FLIP_TIMEOUT)&&(millis()-lastChange>FLIP_DEBOUNCE))
        {
    //        Serial.println("turned to the left and back");
            autoCycle=false;
            lastChange=millis();
            decrementDemo();
            lastLeft=millis()-FLIP_TIMEOUT;
        }
        if(((millis()-lastRight)<FLIP_TIMEOUT)&&(millis()-lastChange>FLIP_DEBOUNCE))
        {
       //     Serial.println("turned to the right and back");
            autoCycle=false;
            lastChange=millis();
            incrementDemo();
            lastRight=millis()-FLIP_TIMEOUT;
        }
    }
    
    if(autoCycle)
        if(millis()-lastAutoCycle>AUTOCYCLE_TIME)   //in autocycle, change demos every 15 seconds
        {
            incrementDemo();
//            Serial.print("autocycling...Demo is ");
//            Serial.println(demo);
            lastAutoCycle=millis();
        }
        
 }
 
void updateAccelerometer()
{
    for(int i=0;i<3;i++)
        accelerometer[i]=analogRead(X+i);
}

void setFadeSpeed()
{
    if(autoCycle)
        fadeSpeed=2;
    else
        fadeSpeed=20;
}
 
 void incrementDemo()
 {
     demo++;
     setFadeSpeed();
     fading=true;
     if(demo>=DEMO_ROUTINES)
        demo=0;
 }
 
  void decrementDemo()
 {
     demo--;
     setFadeSpeed();
     fading=true;
     if(demo<0)
        demo=DEMO_ROUTINES-1;
 }