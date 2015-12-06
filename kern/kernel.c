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
#include <mouse.h>
#include <text.h>
#include <krandom.h>
#include <time.h>
#include <eelphant.h>

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
  mouseinit();
  asm volatile("sti");
  while(time(NULL) == 0);
  krandom_get();
  puts("Welcome to ");
  puts(KERNEL_NAME);
  puts("!\n");
  size_t w = vgetw();
  size_t h = vgeth();
  if(w == 0 || h == 0)
  {
    puts("Cannot start Eelphant: no video\n");
    return;
  }
  eelphant_main(w, h);
  /*vsetcol(255, 0, 0, 255);
  vd_rectangle(FILL, 10, 10, 128, 128);
  int64_t mx = 0;
  int64_t my = 0;
  size_t w = vgetw();
  size_t h = vgeth();
  if(!w) w = 1024;
  if(!h) h = 768;
  rgb_t color;
  color.r = 255;
  color.g = 0;
  color.b = 0;
  color.a = 255;
  enum {RGr, GRf, GBr, BGf, BRr, RBf} color_state = RGr;
  //r = 1
  //r = 1 g++
  //g = 1 r--
  //g = 1 b++
  //b = 1 g--
  //b = 1 r++
  //r = 1 b--
  while(true)
  {
    if(mouseavail())
    {
      while(mouseavail())
      {
      struct mouseevent* e = mousepoll();
      mx += e->dx;
      my += e->dy;
      if(mx < 0) mx = 0;
      if(my < 0) my = 0;
      if(mx >= w) mx = w - 1;
      if(my >= h) my = h - 1;
      switch(color_state)
      {
        case RGr:
          color.g++;
          if(color.g >= 255) color_state = GRf;
          break;
        case GRf:
          color.r--;
          if(color.r <= 0) color_state = GBr;
          break;
        case GBr:
          color.b++;
          if(color.b >= 255) color_state = BGf;
          break;
        case BGf:
          color.g--;
          if(color.g <= 0) color_state = BRr;
          break;
        case BRr:
          color.r++;
          if(color.r >= 255) color_state = RBf;
          break;
        case RBf:
          color.b--;
          if(color.b <= 0) color_state = RGr;
          break;
      }
      vsetcol(color.r, color.g, color.b, color.a);
      vd_rectangle(FILL, mx, my, 32, 32);
      char buffer[64];
      itoa(mx, buffer, 10);
      puts("Mouse x: "); puts(buffer);
      puts(" y: "); itoa(my, buffer, 10); puts(buffer);
      puts(" dx: "); itoa(e->dx, buffer, 10); puts(buffer);
      puts(" dy: "); itoa(e->dy, buffer, 10); puts(buffer);
      putc('\n');
      }
    }
    if(kbdavail())
    {
      struct keyevent* e = kbdpoll();
      putc(e->release ? '^' : '_'); putc(e->character);
    }
    sleep(42);
  }*/
}
