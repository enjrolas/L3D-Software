#include "application.h"
#include <stdarg.h>

#include "colors.h"
#include "math.h"

#include "cube.h"
#include "snow.h"

SYSTEM_MODE(SEMI_AUTOMATIC)

#define PIN_X 14
#define PIN_Y 15
#define PIN_Z 13

void setup() {
    Serial.begin(115200);
    cube_init();

    // initialize snowflakes
    snowstorm();
}

void loop() {
    // read accelerometer
    float accel_x = (float)(analogRead(PIN_X) - 2048) / 2048.0f;
    float accel_y = -(float)(analogRead(PIN_Y) - 2048) / 2048.0f;
    float accel_z = (float)(analogRead(PIN_Z) - 2048) / 2048.0f;

    // favor movement normal to cube floor
    float biased_magnitude = sqrt(pow(accel_x, 2) + pow(accel_z, 2)) + accel_y;

    if(biased_magnitude > -0.08)
        flurry(0.1, 0.02);

    update_snow(accel_x, accel_y, accel_z);

    render_background();
    render_snow();
    cube_update();
}

