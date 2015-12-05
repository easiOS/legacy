/* EasiOS kernel.c
 * ----------------
 * Author(s): Daniel (Easimer) Meszaros
 * ----------------
 * Description: kernel entry point
 */

#include <kernel.h>
#include <stdio.h>
#include <multiboot2.h>
#include <serial.h>
#include <string.h>
#include <dtables.h>
#include <stdlib.h>
#include <timer.h>

const char* cmdline = NULL;

void kpanic(const char* msg, registers_t regs)
{
  puts("-----------------\nEXCEPTION\n-----------------\n");
  asm("cli");
  asm("hlt");
}

void multiboot_enum(uint32_t mbp)
{
  struct multiboot_tag *tag;
  unsigned size;
  size = *(unsigned *)mbp;
  for (tag = (struct multiboot_tag *) (mbp + 8);
  	 tag->type != MULTIBOOT_TAG_TYPE_END;
     tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
        + ((tag->size + 7) & ~7)))
  {
    switch(tag->type)
    {
      case MULTIBOOT_TAG_TYPE_CMDLINE:
			{
				struct multiboot_tag_string *tagstr =
					(struct multiboot_tag_string *)tag;
				cmdline = (tagstr->string);
				if(strcmp(cmdline, "serial") == 0)
				{
					serinit();
          serinitport(COM1);
          puts("Serial enabled from cmdline!\n");
        }
				break;
			}
    }
  }
}

void kmain(uint32_t magic, uint32_t mbp)
{
  /*serinit();
  serinitport(COM1);*/
  multiboot_enum(mbp);
  init_descriptor_tables();
  asm volatile("sti");
  timerinit(1000);
  read_rtc();
  kbdinit();
  while(true);
}
