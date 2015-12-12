#ifndef H_PCI_VBGFX
#define H_PCI_VBGFX

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dev/pci.h>

void vbgfxinit(uint8_t bus, uint8_t slot);
void vbgfx_set_res(uint32_t w, uint32_t h);
int vbgfx_isinit();

#endif
