#ifndef H_SNOW
#define H_SNOW

typedef struct {
    float x, y, z;
} vector;

void flurry(float severity);
void snowstorm(void);
void render_snow(void);
void render_background(void);

void update_snow(void);

#endif
