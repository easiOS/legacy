#ifndef H_GFX
#define H_GFX

#include "gba.h"

#define BLACK 0x0000
#define WHITE 0xffff
#define RED 0x001f
#define GREEN 0x03e0
#define BLUE 0x7c00

typedef enum {FILL, LINE} vdrawmode_t;

void vinit(int32_t width, int32_t height, int32_t bpp, int32_t pitch, uint32_t addr);
void vdestroy(void);
void vcls(void);
void vplot(int16_t x, int16_t y);
void vsetcol(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void vsetcolw(uint16_t w, uint8_t a);
gba_pixel vgetcol(void);
unsigned vgetw(void);
unsigned vgeth(void);
void vd_rectangle(vdrawmode_t drawmode, int16_t x, int16_t y, int16_t w, int16_t h);

#endif /* H_GFX */