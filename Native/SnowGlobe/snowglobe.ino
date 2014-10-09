#include "application.h"

#include <stdarg.h>

#include "colors.h"
#include "math.h"

#include "cube.h"
#include "snow.h"
#include "images.h"

SYSTEM_MODE(SEMI_AUTOMATIC)

#define PIN_X 14
#define PIN_Y 15
#define PIN_Z 13

color body_color = { 3, 3, 2 };

float turbulence = 0;

#define PRINTF_BUFFER_SIZE 128
void Serial_printf(const char* fmt, ...) {
    char buff[PRINTF_BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, PRINTF_BUFFER_SIZE, fmt, args);
    va_end(args);
    Serial.println(buff);
}

void setup() {
    cube_init();

    Serial.begin(115200);

    snowstorm();
}

void loop() {
    // read accelerometer
    float accel_x = (float)(analogRead(PIN_X) - 2048) / 2048.0f;
    float accel_y = -(float)(analogRead(PIN_Y) - 2048) / 2048.0f;
    float accel_z = (float)(analogRead(PIN_Z) - 2048) / 2048.0f;
    
    float magnitude = sqrt(pow(accel_x, 2) + pow(accel_y, 2) + pow(accel_z, 2));

    update_snow(accel_x, accel_y, accel_z);

    // RENDERING

    render_background();

    if(magnitude > 0.21)
        turbulence += magnitude;

    render_snow();

    if(turbulence > 0.1) {
        Serial.println(turbulence);
        flurry(0.05);
        turbulence -= 0.01;
    }

    cube_update();
}

