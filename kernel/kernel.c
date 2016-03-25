#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <time.h>
#include <cpuid.h>
#include <kernel.h>
#include <multiboot2.h>
#include <text.h>
#include <drivers/serial.h>
#include <drivers/timer.h>
#include <memory.h>
#include <video.h>
#include <dtables.h>

uint16_t __attribute__((aligned(4))) text_buffer[2400];
const char* cmdline = NULL;
struct cpu_desc cpu_desc;

void multiboot_enum(void* mbp);
void cpu_check_features();

void kmain(unsigned magic, void* mbp)
{
	cpu_check_features();
	dtables_init();
	multiboot_enum(mbp);
	sinit();
	sinitport(COM1);
	puts("Initializing timer...");
	time_init();
	while(time(NULL) == 0);
	puts("[  OK  ]\n");
	puts("Initializing user I/O...");
	puts("[  OK  ]\n");
	asm volatile("sti");
	puts("Interrupts enabled\n");
	puts("Initializing PCI devices...\n");
	//pciinit();
	puts("PCI devices [  OK  ]\n");
	printf("Welcome to %s\n", KERNEL_NAME);
  	puts("Copyright (c) 2015-2016, Project EasiOS\nAll rights reserved.\n");
  	puts("printf implemetation:\n\tCopyright (c) 2013,2014 Michal Ludvig <michal@logix.cz> All rights reserved.\n");
}

void multiboot_enum(void* mbp)
{
	struct multiboot_tag* tag;
	unsigned int size;
	size = *(unsigned int*)mbp;
	if(size == 0)
		return;
	for(tag = (struct multiboot_tag*)(mbp + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag*)((unsigned short*)tag
			+ ((tag->size + 7) & ~7)))
	{
		switch(tag->type)
		{
			case MULTIBOOT_TAG_TYPE_CMDLINE:
			{
				struct multiboot_tag_string* tagstr =
					(struct multiboot_tag_string*)tag;
				cmdline = (tagstr->string);
				if(strcmp(cmdline, "serial") == 0)
				{
					if(sinitport(COM1) == -1)
					{
						printf("Failed to initialize serial port!\n");
					}
				}
				break;
			}
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
			{
				struct multiboot_tag_framebuffer_common* tagfb =
					(struct multiboot_tag_framebuffer_common*)tag;
				long long width = tagfb->framebuffer_width;
				long long height = tagfb->framebuffer_height;
				long long bpp = tagfb->framebuffer_bpp;
				long long pitch = tagfb->framebuffer_pitch;
				unsigned int addr = tagfb->framebuffer_addr;
				if(tagfb->framebuffer_type == 1)
				{
					vinit(width, height, bpp, pitch, addr);
					tinit(80, 30, (unsigned short*)text_buffer);
				}
				if(tagfb->framebuffer_type == 2)
				{
					tinit(width, height, (unsigned short*)addr);
				}
				break;
			}
			case MULTIBOOT_TAG_TYPE_MMAP:
			{
				struct multiboot_tag_mmap *tagmem =
					(struct multiboot_tag_mmap*)tag;
				int memn = (tagmem->size - 16) / sizeof(struct multiboot_mmap_entry);
				kminit(tagmem->entries, memn);
				break;
			}
			case MULTIBOOT_TAG_TYPE_MODULE:
			{
				printf("Module ignored\n");
				break;
			}
		}
	}
}

void kpanic(const char* msg, registers_t regs)
{
	asm volatile("cli");
	puts("-----------------\nEXCEPTION\n");
	puts(msg); putchar('\n'); puts("-----------------\n");
	puts("Oh golly gee wilikers!");
	hlt_loop:
	cpu_relax();
	goto hlt_loop;
}

void cpu_check_features()
{
  unsigned int a = 0, b = 0, c = 0, d = 0;
  __get_cpuid (0, &a, &b, &c, &d);
  memcpy(&cpu_desc.vendor[0], &b, 4);
  memcpy(&cpu_desc.vendor[4], &d, 4);
  memcpy(&cpu_desc.vendor[8], &c, 4);
  printf("CPU Vendor: %s\n", cpu_desc.vendor);
  __get_cpuid (0x80000000, &a, &b, &c, &d);
  if(a >= 0x80000004)
  {
    for(unsigned int i = 0x80000002; i < 0x80000005; i++)
    {
      __get_cpuid(i, &a, &b, &c, &d);
      memcpy(&cpu_desc.brand[(i - 0x80000002) * 16], &a, 4);
      memcpy(&cpu_desc.brand[(i - 0x80000002) * 16 + 4], &b, 4);
      memcpy(&cpu_desc.brand[(i - 0x80000002) * 16 + 8], &c, 4);
      memcpy(&cpu_desc.brand[(i - 0x80000002) * 16 + 12], &d, 4);
    }
  }
  __get_cpuid (1, &a, &b, &c, &d);
  cpu_desc.features1 = d;
  cpu_desc.features2 = c;
  #define CPUFEATREQ 1
  int cpu_required_features1[CPUFEATREQ] = {bit_SSE};
  for(int i = 0; i < CPUFEATREQ; i++)
  {
    if(!(cpu_desc.features1 & cpu_required_features1[i]))
    {
      registers_t regs;
      kpanic("CPU unsupported", regs);
    }
  }
  extern void sse_enable(void);
  sse_enable();
}