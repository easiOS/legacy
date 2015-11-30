#ifndef H_STDMEM
#define H_STDMEM
#include <stddef.h> /* size_t */
#include <stdint.h>
void memset(void *dest, uint8_t val, uint32_t len);
uint16_t freemem();
#endif
