#ifndef H_GRAPHICS
#define H_GRAPHICS

#include <stddef.h>
#include <stdint.h>

typedef struct vga_card vga_card;

struct vga_card {
  uint16_t vendor, device;
  char name[16];
  uint32_t flags;
  uint32_t data[16];
  uint8_t bus, slot;
  void (*set_mode)(vga_card*, int64_t, int64_t, uint8_t); //set video mode (ptr to card, width, height, bpp)
};

vga_card* graphics_add_card();
void graphics_set_mode(int64_t w, int64_t h, uint8_t bpp);

#endif
