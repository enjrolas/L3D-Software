#ifndef H_SNOW
#define H_SNOW

typedef struct {
    float x, y, z;
} vector;

typedef struct {
    bool stuck;
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
