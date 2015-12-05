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
#include <video.h>
#include <kbd.h>
#include <mem.h>

#define KERNEL_NAME "EasiOS v0.3.0"

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
  char buffer[64];
  puts("Multiboot2 tags: ");
  itoa(size, buffer, 10);
  puts(buffer);
  putc('\n');
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
      case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
			{
				struct multiboot_tag_framebuffer_common* tagfb =
					(struct multiboot_tag_framebuffer_common*) tag;
				puts("Framebuffer");
				puts("\n  Address: ");
        itoa(tagfb->framebuffer_addr, buffer, 16);
        puts(buffer);
				puts(" Pitch: ");
        itoa(tagfb->framebuffer_pitch, buffer, 10);
        puts(buffer);
				puts(" Width: ");
        itoa(tagfb->framebuffer_width, buffer, 10);
        puts(buffer);
				puts(" Height: ");
        itoa(tagfb->framebuffer_height, buffer, 10);
        puts(buffer);
				puts(" Bits per pixel: ");
        itoa(tagfb->framebuffer_bpp, buffer, 10);
        puts(buffer);
        puts(" Type: ");
        itoa(tagfb->framebuffer_type, buffer, 10);
        puts(buffer);
				putc('\n');
        if(tagfb->framebuffer_type == 1)
        {
          vinit(tagfb->framebuffer_width, tagfb->framebuffer_height,
                tagfb->framebuffer_bpp, tagfb->framebuffer_pitch,
                tagfb->framebuffer_addr);
        }
				break;
			}
      case MULTIBOOT_TAG_TYPE_VBE:
      {
        struct multiboot_tag_vbe* tagvbe =
          (struct multiboot_tag_vbe*) tag;
        puts("VBE\n");
        char buffer[64];
        puts(" Mode: ");
        itoa(tagvbe->vbe_mode, buffer, 10);
        puts(buffer);
        puts("\n VBE Interface Seg: 0x");
        itoa(tagvbe->vbe_interface_seg, buffer, 16);
        puts(buffer);
        puts(" Off: 0x");
        itoa(tagvbe->vbe_interface_off, buffer, 16);
        puts(buffer);
        puts(" Len: ");
        itoa(tagvbe->vbe_interface_len, buffer, 10);
        puts(buffer);
        putc('\n');
      }
      case MULTIBOOT_TAG_TYPE_MMAP:
			{
				struct multiboot_tag_mmap *tagmmap =
					(struct multiboot_tag_mmap *)tag;
				int mmap_n = (tagmmap->size - 16) / sizeof(struct multiboot_mmap_entry);
				memmgmt_init(tagmmap->entries, mmap_n);
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
  timerinit(1000);
  read_rtc();
  kbdinit();
  asm volatile("sti");
  puts("Welcome to ");
  puts(KERNEL_NAME);
  puts("!\n");
  while(true)
  {
      if(!kbdavail()) continue;
      struct keyevent* e = kbdpoll();
      int sc = e->keycode;
      uint8_t r = 255,g = 255,b=255;
      if(e->shift)
      {
        g = 100;
      }
      if(e->ctrl)
      {
        b = 100;
      }
      if(e->release)
      {
        r = 0;
        g = 0;
        b = 0;
      }
      for(int y = 0; y < 64; y++)
      {
        for(int x = 0; x < 64; x++)
        {
          vplot(sc%8 * 64 + x, sc/8 * 64 + y, r, g, b);
        }
      }
  }
}
