#include "stdmem.h"
#include <stdint.h>
#include "port.h"

uint16_t freemem()
{
	uint16_t total;
	uint8_t lowmem, highmem;
	outb(0x70, 0x30);
	lowmem = inb(0x71);
	outb(0x70, 0x31);
    highmem = inb(0x71);
    total = lowmem | highmem << 8;
    return total;
}
