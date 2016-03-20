/* EasiOS text.c
 * ----------------
 * Author(s): Daniel (Easimer) Meszaros
 * ----------------
 * Description: text mode terminal
 */

#include <text.h>

size_t tw, th;
size_t tx, ty;
unsigned char tcolor;
unsigned short* tbuffer = 0;

void tinit(size_t width, size_t height, unsigned short* addr)
{
  if(addr == 0)
  {
    return;
  }
  tx = 0;
  ty = 0;
  tw = width;
  th = height;
  tcolor = COLOR_LIGHT_GREY | COLOR_BLACK << 4;
  tbuffer = addr;
  tcls();
}

unsigned char tgetcolor()
{
  return tcolor;
}

void tsetcolor(unsigned char fg, unsigned char bg)
{
  tcolor = fg | bg << 4;
}

int tisinit()
{
  return tbuffer != 0;
}

void tputcat(char a, unsigned char color, size_t x, size_t y)
{
  tbuffer[(y % th) * tw + x] = a | (color << 8);
}

void tscroll()
{
  for(size_t i = 0; i < tw * th; i++)
  {
    tbuffer[i] = tbuffer[i+tw];
  }
  for(size_t i = (tw-1) * th; i <  tw * th; i++)
  {
    tbuffer[i] = 0x20 | tcolor;
  }
  ty = th - 1;
}

void tputc(char a, unsigned char color)
{
  switch(a)
  {
    case '\n':
      ty++;
      if(ty >= th) tscroll();
      tx = 0;
      return;
    case '\b':
      tbuffer[ty * tw + tx] = ' ' | tcolor;
      tx--;
      if(tx < 0)
      {
        tx = 0;
        ty--;
      }
      if(ty < 0) ty = 0;
      return;
  }
  tputcat(a, color, tx, ty);
  tx++;
  if(tx >= tw)
  {
    tx = 0;
    ty++;
  }
  if(ty >= th)
  {
    tscroll();
  }
}
void tcls()
{
  for(size_t y = 0; y < th; y++)
    for(size_t x = 0; x < tw; x++)
    {
      tbuffer[y * tw + x] = ' ' | tcolor;
    }
}
void tswrite(const char* str)
{
  for(int i = 0; str[i] != '\0'; i++)
  {
    tputc(str[i], tcolor);
  }
}

const unsigned short* tgetbuf()
{
  return tbuffer;
}
