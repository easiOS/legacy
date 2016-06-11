#include "gba.h"
#include "video.h"

unsigned short* fb = NULL;
gba_pixel color;
unsigned fbw, fbh, fbbpp, fbp;

void vinit(int32_t width, int32_t height, int32_t bpp, int32_t pitch, uint32_t addr)
{
	*GBA_IO = 0x0403;
	fb = (unsigned short*)0x06000000;
	fbw = 240;
	fbh = 160;
	fbbpp = 0;
	fbp = 0;
	color = 0x0000;
	for(unsigned short y = 0; y < 160; y++)
		for(unsigned short x = 0; x < 240; x++)
			fb[y * 240 + x] = WHITE;
}

void vdestroy(void)
{
	fb = NULL;
	fbw = 0; fbh = 0; fbbpp = 0; fbp = 0;
	color = BLACK;
}

void vplot(int16_t x, int16_t y)
{
	if(!fb)
		return;
	if(x < 0 || y < 0)
		return;
	if(x >= fbw || y >= fbh)
		return;

	fb[y * 240 + x] = color;
}

void vsetcol(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	color = ((b/8) << 10) | ((g/8) << 5) | (r/8);
}

void vsetcolw(uint16_t w, uint8_t a)
{
	color = w;
}

gba_pixel vgetcol(void)
{
	return color;
}

void vcls()
{
  for(int16_t y = 0; y < fbh; y++)
    for(int16_t x = 0; x < fbw; x++)
       vplot(x, y);
}

unsigned vgetw(void)
{
	return fbw;
}

unsigned vgeth(void)
{
	return fbh;
}

void vd_rectangle(vdrawmode_t drawmode, int16_t x, int16_t y, int16_t w, int16_t h)
{
  if(drawmode == FILL)
    for(int16_t iy = y; iy < y + h; iy++)
      for(int16_t ix = x; ix < x + w; ix++)
        vplot(ix, iy);
  else if(drawmode == LINE)
  {
    for(int16_t ix = x; ix < x + w; ix++)
    {
      vplot(ix, y);
      vplot(ix, y + h);
    }
    for(int16_t iy = y; iy < y + h; iy++)
    {
      vplot(x, iy);
      vplot(x + w, iy);
    }
  }
}

void vd_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
  float rx1, rx2, ry1, ry2;
  if(x2 >= x1)
  {
    rx1 = x1;
    rx2 = x2;
    ry1 = y1;
    ry2 = y2;
  }
  else
  {
    rx1 = x2;
    rx2 = x1;
    ry1 = y2;
    ry2 = y1;
  }
  float dx = rx2 - rx1;
  float dy = ry2 - ry1;
  for(float x = rx1; x < rx2; x += 0.5)
  {
    float y = ry1 + dy * (x - rx1) / dx;
    vplot((int16_t)x, (int16_t)y);
  }
}