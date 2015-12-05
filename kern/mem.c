#include <mem.h>
#include <stdio.h>
#include <stdlib.h>

void memmgmt_init(struct multiboot_mmap_entry* mmap, int mmap_size)
{
  int found = 0;
  for(int i = 0; i < mmap_size; i++)
  {
    if(mmap[i].type == 1)
    {
      if(found)
      {
        mmgmt_conf.address = mmap[i].addr;
        mmgmt_conf.blocks_n = mmap[i].len / 16384;
        if(mmgmt_conf.blocks_n > 2048)
        {
          mmgmt_conf.blocks_n = 2048;
        }
      }
      else
      {
        found = 1;
      }
    }
    //mmap_entry += (mmap_entry->len + sizeof(struct multiboot_mmap_entry));
  }
  if(found && mmgmt_conf.address == 0)
  {
    puts("Memory Manager init failed, free block not found\n");
  }
  else
  {
    char buffer[64];
    puts("Memory Manager set up with parameters: \n  Address: ");
    itoa(mmgmt_conf.address, buffer, 16);
    puts(buffer);
    puts("\n  Number of 16k blocks: ");
    itoa(mmgmt_conf.blocks_n, buffer, 10);
    puts(buffer);
    putc('\n');
  }
}
