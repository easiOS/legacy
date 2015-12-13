#ifndef H_PCI_VBGFX
#define H_PCI_VBGFX

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dev/pci.h>
#include <dev/graphics.h>
#include <string.h>

void vbgfxinit(uint8_t bus, uint8_t slot);
void vbgfx_set_res(vga_card* v, int64_t w, int64_t h, uint8_t bpp);
int vbgfx_isinit();

#endif
