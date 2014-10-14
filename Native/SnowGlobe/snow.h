#ifndef H_SNOW
#define H_SNOW

typedef struct {
    float x, y, z;
} vector;

enum {
    SNOW_STATIC = 1 << 0, // never move
    SNOW_STUCK  = 1 << 1, // don't move now
    SNOW_THICK  = 1 << 2  // always render particle at home position
};

typedef struct {
    uint8_t flags;
    float x, y, z;
    float vx, vy, vz;
    float home_x, home_y, home_z;
} snowflake;

void flurry(float severity, float strength);
void snowstorm(void);
void render_snow(void);
void render_background(void);

void update_snow(float ax, float ay, float az);

#endif
