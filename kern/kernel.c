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
#include <dev/net_loopback.h>
#include <kshell.h>
#include <users.h>
#include <vfs.h>
#include <acpi.h>
#include <stdarg.h>

#define KERNEL_NAME "EasiOS v0.3.2"

const char* cmdline = NULL;
uint16_t text_buffer[2000];

size_t eos_write(int fd, void* buf, size_t len)
{
  size_t ret = 0;
  if(fd == 1)
  {
    char* cbuf = (char*)buf;
    int i;
    for(i = 0; i < len; i++)
    {
      putc(cbuf[i]);
    }
    ret = i;
  }
  return ret;
}

asm("eos_syscall_start:");

int eos_syscall(uint32_t id, ...)
{
  //puts("SYSCALL!!!!\n");
  int ret = 0;
  va_list ap;
  va_start(ap, id);
  switch(id)
  {
    case 0: //write
    {
      ret = eos_write(va_arg(ap, int), va_arg(ap, void*), va_arg(ap, size_t));
      break;
    }
  }
  va_end(ap);
  return ret;
}

asm("eos_syscall_end:");

void kpanic(const char* msg, registers_t regs)
{
  asm volatile("cli");
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
  puts("drawn rectangle\n");
  vswap();
  puts("swapped\n");
  int64_t fx, fy;
  vd_print(100, 250, "Your PC ran into a problem and needs a restart.", &fx, &fy);
  vd_print(100, fy + 20, "We're just collecting some error info and then you can restart.", &fx, &fy);
  vswap();
  puts("swapped\n");
  int64_t px, py;
  vd_print(100, fy + 30, "If you'd like to know more, you can search online later for this error: ", &px, &py);
  vswap();
  puts("swapped\n");
  vd_print(100, py + 20, msg, NULL, NULL);
  vswap();
  hlt_loop:
  asm("hlt");
  goto hlt_loop;
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
          tinit(80, 25, (uint16_t*)&text_buffer);
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
          puts("  EasiOS VFS detected\n");
          vfs_process((void*)tagmod->mod_start);
          continue;
        }
        if(*(uint32_t*)tagmod->mod_start == 0x0237C0C0)
        {
          puts("  EasiOS Userfile detected\n");
          eos_users_init((struct eos_user_header*)tagmod->mod_start);
          continue;
        }
        if(*(uint32_t*)tagmod->mod_start == 0x1B4C7561)
        {
          puts("  Lua bytecode detected\n");
          //TODO: load
          continue;
        }
        struct posix_header* tarmod = (struct posix_header*)tagmod->mod_start;
        if(tarmod->magic[0] == 'u' && tarmod->magic[1] == 's' &&
           tarmod->magic[2] == 't' && tarmod->magic[3] == 'a' &&
           tarmod->magic[4] == 'r')
        {
          puts("  Tar file detected\n");
        }
        break;
      }
    }
  }
}

extern void topkek();

void kmain(uint32_t magic, uint32_t mbp)
{
  char buffer[64];
  multiboot_enum(mbp);
  puts("Detecting ACPI...");
  struct rsdp_desc* rsdp_p = acpi_findrsdp();
  if(rsdp_p)
  {
    printf("Found: %x\n", (uint32_t)rsdp_p);
    puts("OEM string: ");
    for(int i = 0; i < 6; i++)
    {
      putc(rsdp_p->oemid[i]);
    }
    putc('\n');
  }
  else
  {
    puts("not found.\n");
  }
  /*int len;
  asm ("movl eos_syscall_end - eos_syscall_start, %%eax\n\t"
       "movl %%eax, %0\n\t"
       :"=r"(len)
       :
       :"%eax"
      );
  memcpy((void*)0x100, (void*)&eos_syscall, len);*/
  init_descriptor_tables();
  timerinit(1000);
  read_rtc();
  nlb_init();
  kbdinit();
  mouseinit();
  //ideinit();
  pciinit();
  asm volatile("sti");
  while(time(NULL) == 0);
  krandom_get();
  ethernet_list();
  printf("Welcome to %s!\n", KERNEL_NAME);
  puts("Copyright (c) 2015, Daniel (Easimer) Meszaros\nAll rights reserved.\n");
  //ping test
  uint8_t localip[4] = {127, 0, 0, 1};
  icmp_send_ping_req(localip, localip);
  size_t w = vgetw();
  size_t h = vgeth();
  if(w == 0 || h == 0)
  {
    puts("Cannot start Eelphant: no video. Falling back to kernel shell\n");
    kshell_main();
    puts("kshell returned\n"); return;
  }
  int ret = 0;
  do
  {
    ret = eelphant_main(w, h);
  } while (ret);
  printf("eelphant returned with %d\n", ret);
  reboot("Eelphant exited\n");
}

void reboot(const char* reason)
{
  uint8_t temp;
  puts("\n\n");
  puts("The system is going down for reboot NOW!\n");
  puts(reason); puts("\n\n\n");
  uint64_t now = ticks();
  while(ticks() - now < 2000);
  asm volatile("cli");
  do {
    temp = inb(0x64);
    if((temp & 1) != 0)
      inb(0x60);
  } while((temp & 2) != 0);
  outb(0x64, 0xFE);
  loop:
  asm volatile("hlt");
  goto loop;
}
