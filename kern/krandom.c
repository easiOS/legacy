#include <krandom.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

static uint32_t kr_next = -1;

void krandom_add(uint32_t n)
{
  kr_next += n;
}

uint32_t krandom_get()
{
  if(kr_next == -1)
  {
    kr_next = time(NULL);
    char buffer[64];
    itoa(kr_next, buffer, 10);
    puts("krandom seeded with ");
    puts(buffer);
    putc('\n');
  }
  kr_next = kr_next * 1103515245 + time(NULL);
  return ((kr_next/65536) % 32768) % 11;
}
