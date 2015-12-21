#include <dev/graphics.h>

#define MAX_GFX_N 1

vga_card gfx_devices[MAX_GFX_N];

vga_card* graphics_add_card()
{
  vga_card* ret = NULL;
  for(int i = 0; i < MAX_GFX_N; i++)
  {
    if((gfx_devices[i].flags & 1) == 0)
    {
      gfx_devices[i].flags |= 1;
      ret = &gfx_devices[i];
      break;
    }
  }
  return ret;
}

void graphics_set_mode(int64_t w, int64_t h, uint8_t bpp)
{
  for(int i = 0; i < MAX_GFX_N; i++)
  {
    gfx_devices[i].set_mode(&gfx_devices[i], w, h, bpp);
  }
}

void graphics_swap_buffer(void* ptr)
{

}

int graphics_available()
{
  int ret = 0;
  for(int i = 0; i < MAX_GFX_N; i++)
  {
    if(gfx_devices[i].vendor)
    {
      ret = 1;
      break;
    }
  }
  return ret;
}
