#include <stdlib.h>
#include <string.h>
#include <mem.h>
#include <stdio.h>
#include <stdbool.h>

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
  puts("Enter malloc\n");
  if(size <= 0){ puts("malloc: Size less than zero\n"); return 0;}
  /*uint32_t db = size / 16384 + 1;
  int n = -1;
  for(int i = 0; i < mmgmt_conf.blocks_n; i++)
  {
    bool c = true;
    for(int j = 0; j < db; j++)
    {
      if(mmgmt_conf.blocks[i + j] & 1)
      {
        c = false;
        break;
      }
    }
    if(c)
    {
      puts("malloc: found ");
      char b[16];
      itoa(db, b, 10);
      puts(b); puts(" free blocks\n");
      for(int j = 0; j < db; j++)
      {
        mmgmt_conf.blocks[i + j] |= 1;
      }
      n = i;
      break;
    }
  }
  uint32_t address = 0;
  if(n != -1)
  {
    puts("malloc: n: ");
    char b[16];
    itoa(n, b, 10);
    puts(b); puts("\n");
    mmgmt_conf.blocks[n] |= db << 1;
    address = mmgmt_conf.address + (n * 16384);
  }
  else
    puts("malloc(): out of memory\n");*/
  uint32_t address = (uint32_t)mmgmt_alloc(size);
  puts("Exit malloc\n");
  return (void*)address;
}

void* free(void* ptr) //actually this is kfree but who cares lol
{
  /*if(ptr == 0) return 0;
  if((int)ptr < mmgmt_conf.address || (int)ptr > mmgmt_conf.address + mmgmt_conf.blocks_n * 16384)
  {
    puts("free(): ptr points to unmanaged memory area\n");
    return ptr;
  }
  uint32_t tmp = (uint32_t)ptr;
  uint32_t block_i = (tmp - mmgmt_conf.address) / 16384;
  uint64_t nextn = mmgmt_conf.blocks[block_i] >> 1;
  char b[16];
  puts("free: block_i: ");
  itoa(block_i, b, 10);
  puts(b);
  putc('\n');
  itoa(nextn, b, 10);
  puts("free: next ");
  puts(b);
  puts(" blocks needs to be freed too\n");
  for(size_t i = block_i; i < block_i + nextn; i++)
  {
      mmgmt_conf.blocks[i] = 0; //clear allocated bit
  }*/
  mmgmt_free(ptr);
  return ptr;
}
