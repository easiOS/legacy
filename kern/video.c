/* EasiOS kernel.c
 * ----------------
 * Author(s): Daniel (Easimer) Meszaros
 * ----------------
 * Description: Graphics, drawing, fonts
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <video.h>

uint32_t* fb;
size_t fbw, fbh, fbbpp, fbp;
uint8_t fbt;
rgb_t color;

void vinit(size_t width, size_t height, size_t bpp, size_t pitch, uint64_t addr)
{
  fb = (uint32_t*)(uint32_t)addr;
  fbw = width;
  fbh = height;
  fbbpp = bpp;
  fbp = pitch;
  color.r = 255;
  color.g = 255;
  color.b = 255;
}

void vplot(size_t x, size_t y)
{
  if(x >= fbw) return;
  if(y >= fbh) return;
  fb[y * fbw + x] = color.r << 16 | color.g << 8 | color.b;
}

void vsetcol(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  color.r = r;
  color.b = b;
  color.g = g;
  color.a = a;
}

rgb_t vgetcol()
{
  return color;
}

void vcls()
{
  for(size_t y = 0; y < fbh; y++)
    for(size_t x = 0; x < fbw; x++)
      vplot(x, y);
}

void vd_print(size_t x, size_t y, const char* str)
{

}

size_t vgetw()
{
  return fbw;
}

size_t vgeth()
{
  return fbh;
}

void vd_rectangle(vdrawmode_t drawmode, size_t x, size_t y, size_t w, size_t h)
{
  if(drawmode == FILL)
    for(size_t iy = y; iy < y + h; iy++)
      for(size_t ix = x; ix < x + w; ix++)
        vplot(ix, iy);
  else if(drawmode == LINE)
  {
    for(size_t ix = x; ix < x + w; ix++)
    {
      vplot(ix, y);
      vplot(ix, y + h);
    }
    for(size_t iy = y; iy < y + h; iy++)
    {
      vplot(x, iy);
      vplot(x + w, iy);
    }
  }
}

void vd_line(size_t x1, size_t y1, size_t x2, size_t y2)
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
    vplot((size_t)x, (size_t)y);
  }
}
