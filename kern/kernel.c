/* EasiOS kernel.c
 * ----------------
 * Author(s): Daniel (Easimer) Meszaros
 * ----------------
 * Description: kernel entry point
 */

#include <kernel.h>
#include <stdio.h>
#include <multiboot2.h>
#include <dev/serial.h>
#include <string.h>
#include <dtables.h>
#include <stdlib.h>
#include <dev/timer.h>
#include <video.h>
#include <dev/kbd.h>
#include <mem.h>
#include <dev/mouse.h>
#include <text.h>
#include <krandom.h>
#include <time.h>
#include <eelphant.h>
#include <dev/ide.h>
#include <dev/pci.h>
#include <tar.h>
#include <dev/ethernet.h>

#define KERNEL_NAME "EasiOS v0.3.1"

const char* cmdline = NULL;

void kpanic(const char* msg, registers_t regs)
{
  puts("-----------------\nEXCEPTION\n");
  puts(msg); putc('\n'); puts("-----------------\n");
  vsetcol(0x20, 0x67, 0xb2, 0xff);
  vcls();
  vsetcol(0xff, 0xff, 0xff, 0xff);
  vd_rectangle(FILL, 100, 100, 32, 32);
  vd_rectangle(FILL, 100, 200, 32, 32);
  vd_rectangle(FILL, 196, 100, 16, 132);
  struct {int x,y,w,h;} ayy[] = {{212, 100, 32, 16}, {244, 116, 32, 16},
  {276, 132, 16, 64},
  {212, 216, 32, 16}, {244, 200, 32, 16}};
  for(int i = 0; i < 5; i++)
    vd_rectangle(FILL, ayy[i].x, ayy[i].y, ayy[i].w, ayy[i].h);
  vswap();
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
        if(tagfb->framebuffer_type == 2)
        {
          tinit(tagfb->framebuffer_width, tagfb->framebuffer_height,
                (uint16_t*)(uint32_t)tagfb->framebuffer_addr);
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
        break;
      }
      case MULTIBOOT_TAG_TYPE_MMAP:
			{
				struct multiboot_tag_mmap *tagmmap =
					(struct multiboot_tag_mmap *)tag;
				int mmap_n = (tagmmap->size - 16) / sizeof(struct multiboot_mmap_entry);
				memmgmt_init(tagmmap->entries, mmap_n);
        break;
			}
      case MULTIBOOT_TAG_TYPE_MODULE:
      {
        struct multiboot_tag_module *tagmod =
          (struct multiboot_tag_module *)tag;
        puts("GRUB module detected!\n");
        if(*(uint32_t*)tagmod->mod_start == 0xC0C0A123)
        {
          puts("EasiOS VFS detected\n");
          //TODO: load
          continue;
        }
        struct posix_header* tarmod = (struct posix_header*)tagmod->mod_start;
        if(tarmod->magic[0] == 'u' && tarmod->magic[1] == 's' &&
           tarmod->magic[2] == 't' && tarmod->magic[3] == 'a' &&
           tarmod->magic[4] == 'r')
        {
          puts("Valid tar file\n");
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
  timerinit(1000);
  read_rtc();
  kbdinit();
  mouseinit();
  //ideinit();
  pciinit();
  asm volatile("sti");
  while(time(NULL) == 0);
  krandom_get();
  ethernet_list();
  puts("Welcome to ");
  puts(KERNEL_NAME);
  puts("!\n");
  puts("Copyright (c) 2015, Daniel (Easimer) Meszaros\nAll rights reserved.\n");
  size_t w = vgetw();
  size_t h = vgeth();
  if(w == 0 || h == 0)
  {
    puts("Cannot start Eelphant: no video\n");
    return;
  }
  eelphant_main(w, h);
}
