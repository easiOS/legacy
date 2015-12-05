#include <memmgmt.h>
#include <video.h>

void memmgmt_init(struct multiboot_mmap_entry* mmap, int mmap_size)
{
  int found = 0;
  for(int i = 0; i < mmap_size; i++)
  {
    terminal_writestring("Address: "); terminal_writeuint(mmap[i].addr);
    terminal_writestring(" Length: "); terminal_writeuint(mmap[i].len);
    terminal_writestring(" Type: "); terminal_writeuint(mmap[i].type);
    terminal_writestring(" SBZ: "); terminal_writeuint(mmap[i].zero);
    terminal_putchar('\n');
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
    terminal_writestring("Memory Manager init failed, free block not found\n");
  }
  else
  {
    terminal_writestring("Memory Manager set up with parameters: \n  Address: ");
    terminal_writeint(mmgmt_conf.address);
    terminal_writestring("\n  Number of 16k blocks: ");
    terminal_writeint(mmgmt_conf.blocks_n);
    terminal_putchar('\n');
  }
}
