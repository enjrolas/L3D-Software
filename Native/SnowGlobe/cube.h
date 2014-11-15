#ifndef H_CUBE
#define H_CUBE

static color color_dark = { 0, 0, 0 };

void cube_init(void);
void cube_update(void);
void setPixel(int x, int y, int z, color* col);
color getPixel(int x, int y, int z);

#endif
