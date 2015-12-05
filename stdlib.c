#include <stdlib.h>
#include <memmgmt.h>
#include <dtables.h>
#include <kernel.h>
#include <string.h>
#include <timer.h>
#include <video.h>

void* malloc(size_t size)
{
  if(size <= 0) return 0;
  if(size > 16384)
  {
    terminal_prfxi(ticks(), "malloc(): fuck\n");
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
    terminal_prfxi(ticks(), "malloc(): out of memory\n");
  return (void*)address;
}

void* free(void* ptr)
{
  if(ptr == 0) return 0;
  if((int)ptr < mmgmt_conf.address || (int)ptr > mmgmt_conf.address + mmgmt_conf.blocks_n * 16384)
  {
    terminal_prfxi(ticks(), "free(): ptr points to unmanaged memory area\n");
    return ptr;
  }
  uint32_t tmp = (uint32_t)ptr;
  uint32_t block_i = (tmp - mmgmt_conf.address) / 16384;
  terminal_prfxi(ticks(), "free(): freeing block #");
  terminal_writeuint(block_i - 1);
  terminal_putchar('\n');
  mmgmt_conf.blocks[block_i - 1] &= ~1; //clear allocated bit
  return ptr;
}
