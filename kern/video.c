/* EasiOS kernel.c
 * ----------------
 * Author(s): Daniel (Easimer) Meszaros
 * ----------------
 * Description: VBE
 */

#include <stdint.h>
#include <stddef.h>

uint32_t* fb;
size_t fbw, fbh, fbbpp, fbp;
uint8_t fbt;

void vinit(size_t width, size_t height, size_t bpp, size_t pitch, uint64_t addr)
{
  fb = (uint32_t*)(uint32_t)addr;
  fbw = width;
  fbh = height;
  fbbpp = bpp;
  fbp = pitch;
}

void vplot(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b)
{
  fb[y * fbw + x] = r << 16 | g << 8 | b;
}
