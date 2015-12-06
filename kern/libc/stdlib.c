#include <stdlib.h>
#include <string.h>
#include <mem.h>

char* itoa(int64_t n, char* s, int base)
{
  char tmp[33];
  memset(tmp, 0, 33);
  char *tp = tmp;
  int i;
  unsigned v;

  int sign = (base == 10 && n < 0);
  if (sign)
      v = -n;
  else
      v = (unsigned)n;

  while (v || tp == tmp)
  {
      i = v % base;
      v /= base;
      if (i < 10)
        *tp++ = i+'0';
      else
        *tp++ = i + 'a' - 10;
  }

  int len = tp - tmp;

  if (sign)
  {
      *s++ = '-';
      len++;
  }

  while (tp > tmp)
      *s++ = *--tp;
  *s = '\0';
  return s;
}

void* malloc(size_t size)
{
  if(size <= 0) return 0;
  if(size > 16384)
  {
    puts("malloc(): fuck\n");
  }
  int n = -1;
  for(int i = 0; i < mmgmt_conf.blocks_n; i++)
  {
    if(!(mmgmt_conf.blocks[i] & 1)) //is block non-allocated
    {
      mmgmt_conf.blocks[i] |= 1;
      n = i;
      break;
    }
  }
  uint32_t address = 0;
  if(n != -1)
    address = mmgmt_conf.address + (n * 16384);
  else
    puts("malloc(): out of memory\n");
  return (void*)address;
}

void* free(void* ptr)
{
  if(ptr == 0) return 0;
  if((int)ptr < mmgmt_conf.address || (int)ptr > mmgmt_conf.address + mmgmt_conf.blocks_n * 16384)
  {
    puts("free(): ptr points to unmanaged memory area\n");
    return ptr;
  }
  uint32_t tmp = (uint32_t)ptr;
  uint32_t block_i = (tmp - mmgmt_conf.address) / 16384;
  mmgmt_conf.blocks[block_i - 1] &= ~1; //clear allocated bit
  return ptr;
}
