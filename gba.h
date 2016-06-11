#ifndef H_GBA
#define H_GBA

#include <stddef.h>
#include <stdint.h>

#define GBA_IO ((unsigned int*)0x04000000)
#define GBA_VRAM ((unsigned short*)0x06000000)

typedef uint16_t gba_pixel; //BGR 5.5.5

#endif /* H_GBA */