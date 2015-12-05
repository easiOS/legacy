#define KERNEL_NAME "EasiOS "
#define KERNEL_VERSION "0.2.0 "

#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
#include "multiboot2.h"
#include "kernel.h"
#include "stdmem.h"
#include "video.h"
#include "port.h"
#include "dtables.h"
#include "timer.h"
#include "keyboard.h"
#include "timer.h"
#include "mouse.h"
#include "itoa.h"
#include "serial.h"
#include "ps2.h"
#include "tar.h"
//#include "realvideo.h"
#include "shell/shell.h"
#include <string.h>
#include <memmgmt.h>
#include <stdlib.h>

#define ACK 0xFA
#define RES 0xFE

#define KEYB_DAT 0x60
#define KEYB_CMD 0x64

short palfy[18] = {0b11111100, 0b0010100, 0b11100,
	0, 0b11101000, 0b10101010, 0b11111000,
	0, 0b11111000, 0, 0b11111000, 0b101000, 0b101000,
	0b1000, 0, 0b111000, 0b100000, 0b11111000};

uint32_t boot_time[6];
const char* cmdline = NULL;

uint32_t* get_boot_date()
{
	return boot_time;
}

bool get_nth_binary(short binary, int nth)
{
	return (bool)((binary >> nth) & 1);
}

void halt()
{
	terminal_writestring("Processor halted.");
	asm volatile("cli");
	asm volatile("hlt");
}

void kpanic(const char* msg, registers_t regs)
{
	uint8_t color = make_color(15, 1);
	for(size_t y = 0; y < 25; y++)
	{
		for(size_t x = 0; x < 80; x++)
		{
			terminal_putentryat(terminal_getcharat(x, y), color, x, y);
		}
	}
	terminal_setcolor(color);
	terminal_writestring("\nFatal Error: ");
	terminal_writestring(msg);
	terminal_writestring("\n");
	//ds, edi, esi, ebp, esp, ebx, edx, ecx, eax;
	terminal_writestring("DS="); terminal_writeint(regs.ds); terminal_writestring("\n");
	terminal_writestring("EDI="); terminal_writeint(regs.edi); terminal_writestring("\n");
	terminal_writestring("ESI="); terminal_writeint(regs.esi); terminal_writestring("\n");
	terminal_writestring("EBP="); terminal_writeint(regs.ebp); terminal_writestring("\n");
	terminal_writestring("ESP="); terminal_writeint(regs.esp); terminal_writestring("\n");
	terminal_writestring("EBX="); terminal_writeint(regs.ebx); terminal_writestring("\n");
	terminal_writestring("EDX="); terminal_writeint(regs.edx); terminal_writestring("\n");
	terminal_writestring("ECX="); terminal_writeint(regs.ecx); terminal_writestring("\n");
	terminal_writestring("EAX="); terminal_writeint(regs.eax); terminal_writestring("\n");
	//eip, cs, eflags, useresp, ss
	terminal_writestring("EIP="); terminal_writeint(regs.eip); terminal_writestring("\n");
	terminal_writestring("CS="); terminal_writeint(regs.cs); terminal_writestring("\n");
	terminal_writestring("EFLAGS="); terminal_writeint(regs.eflags); terminal_writestring("\n");
	terminal_writestring("USERESP="); terminal_writeint(regs.useresp); terminal_writestring("\n");
	terminal_writestring("SS="); terminal_writeint(regs.ss); terminal_writestring("\n");
	terminal_writestring("Interrupt: "); terminal_writeint(regs.int_no); terminal_writestring("\n");
	terminal_writestring("Error code: "); terminal_writeint(regs.err_code); terminal_writestring("\n");
	halt();
}

void reboot()
{
	terminal_clear();
	terminal_writestring("The system is going down for reboot NOW!\n");
	sleep(750);
    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
    halt();
}

void switch_to_user_mode()
{
   // Set up a stack structure for switching to user mode.
   asm volatile("  \
     cli; \
     mov $0x23, %ax; \
     mov %ax, %ds; \
     mov %ax, %es; \
     mov %ax, %fs; \
     mov %ax, %gs; \
                   \
     mov %esp, %eax; \
     pushl $0x23; \
     pushl %eax; \
     pushf; \
     pushl $0x1B; \
     push $1f; \
     iret; \
   1: \
     ");
   terminal_writestring("So this is user mode. The grass was\nmuch greener on the other site :(\n");
}

void logo()
{
	//int ox = terminal_getx();
	int oy = terminal_gety();
	uint8_t color = COLOR_LIGHT_GREY | COLOR_BLACK << 4;
	for(int i = 0; i < 18; i++)
	{
		for(int j = 0; j < 8; j++)
		{
			if(get_nth_binary(palfy[i], j))
				terminal_putentryat('*', color, i, j + oy);
		}
	}
	terminal_setcursor(0, 9 + oy);
}

void parse_multiboot2_tags(unsigned long mboot_ptr)
{
	struct multiboot_tag *tag;
	unsigned size;
	size = *(unsigned *) mboot_ptr;
	terminal_writestring("Multiboot2 addr "); terminal_writeint(mboot_ptr);
	terminal_writestring(" size "); terminal_writeint(size);
	terminal_putchar('\n');
	for (tag = (struct multiboot_tag *) (mboot_ptr + 8);
		 tag->type != MULTIBOOT_TAG_TYPE_END;
		 tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag
						 + ((tag->size + 7) & ~7)))
	{
		terminal_writestring("Tag type "); terminal_writeint(tag->type);
		terminal_writestring(" size "); terminal_writeint(tag->size);
		terminal_putchar('\n');
		switch (tag->type)
		{
			case MULTIBOOT_TAG_TYPE_CMDLINE:
			{
				struct multiboot_tag_string *tagstr =
					(struct multiboot_tag_string *)tag;
				terminal_writestring("Command line: "); terminal_writestring(tagstr->string);
				terminal_putchar('\n');
				cmdline = (tagstr->string);
				if(strcmp(cmdline, "serial") == 0)
				{
					terminal_writestring("Serial enabled from cmdline!\n");
					init_serial();
				}
				break;
			}
			case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
			{
				struct multiboot_tag_framebuffer_common* tagfb =
					(struct multiboot_tag_framebuffer_common*) tag;
				terminal_writestring("Framebuffer");
				terminal_writestring("\n  Address: "); terminal_writeint(tagfb->framebuffer_addr);
				terminal_writestring(" Pitch: "); terminal_writeint(tagfb->framebuffer_pitch);
				terminal_writestring(" Width: "); terminal_writeint(tagfb->framebuffer_width);
				terminal_writestring(" Height: "); terminal_writeint(tagfb->framebuffer_height);
				terminal_writestring(" Bits per pixel: "); terminal_writeint(tagfb->framebuffer_bpp);
				terminal_putchar('\n');
				break;
			}
			case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
			{
				struct multiboot_tag_basic_meminfo* tagmem =
					(struct multiboot_tag_basic_meminfo*) tag;
				terminal_writestring("Lower: "); terminal_writeint(tagmem->mem_lower);
				terminal_writestring(" kilobytes\nUpper: "); terminal_writeint(tagmem->mem_upper);
				terminal_writestring(" kilobytes\n");
				break;
			}
			case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
			{
				struct multiboot_tag_string *tagstr =
					(struct multiboot_tag_string *)tag;
				terminal_writestring("Bootloader name: "); terminal_writestring(tagstr->string);
				terminal_putchar('\n');
				break;
			}
			case MULTIBOOT_TAG_TYPE_MODULE:
			{
				struct multiboot_tag_module *tagmod =
					(struct multiboot_tag_module *)tag;
				terminal_writestring("Start: "); terminal_writeint(tagmod->mod_start);
				terminal_writestring("\nEnd: "); terminal_writeint(tagmod->mod_end);
				terminal_writestring(" "); terminal_writestring(tagmod->cmdline);
				terminal_putchar('\n');
				struct posix_header* tarmod = (struct posix_header*)tagmod->mod_start;
				if(tarmod->magic[0] != 'u' ||
					 tarmod->magic[1] != 's' ||
				 	 tarmod->magic[2] != 't' ||
				   tarmod->magic[3] != 'a' ||
				   tarmod->magic[4] != 'r')
				{
					terminal_writestring("Module is not tar file\n");
				}
				else
				{
					int size = atoi(tarmod->size);
					terminal_writestring("Module size: ");
					terminal_writeint(size);
					terminal_putchar('\n');
				}
				break;
			}
			case MULTIBOOT_TAG_TYPE_MMAP:
			{
				struct multiboot_tag_mmap *tagmmap =
					(struct multiboot_tag_mmap *)tag;
				terminal_writestring("Size: ");
				int mmap_n = (tagmmap->size - 16) / sizeof(struct multiboot_mmap_entry);
				terminal_writeint(mmap_n);
				terminal_putchar('\n');
				memmgmt_init(tagmmap->entries, mmap_n);

			}
			default:
				break;
		}
	}
	terminal_writestring("End enumerating tags\n");
}

void kernel_main(unsigned long magic, unsigned long mboot_ptr)
{
	terminal_initialize();
	init_descriptor_tables();
	//terminal_clear();
	asm volatile("sti");
	//multiboot
	if((int)mboot_ptr!=0)
	{
		if(magic != MULTIBOOT2_BOOTLOADER_MAGIC)
		{
			terminal_writestring("FATAL: Bad magic\n");
			return;
		}
		if(mboot_ptr & 7)
		{
			terminal_writestring("FATAL: Unaligned mbi\n");
			return;
		}
		parse_multiboot2_tags(mboot_ptr);
	}
	else
	{
		terminal_writestring("No bootloader :(\n");
	}
	int control;
	asm("\t movl %%cr0,%0" : "=r"(control));
	if(control & 1)
	{
		terminal_writestring("Protected mode!!\n");
	}
	else
	{
		terminal_writestring("Real mode!!\n");
	}
	init_timer(1000); //1000 Hz
	//init_ps2();
	keyb_init();
	read_rtc();
	//init_mouse();
	//logo();
	terminal_writestring(KERNEL_NAME);
	terminal_writestring(KERNEL_VERSION);
	#ifdef KERNEL_GIT_HEAD
	terminal_writestring(KERNEL_GIT_HEAD);
	#endif
	terminal_writestring("\n");
	uint32_t* t = get_time();
	for(int i = 0; i < 6; i++)
	{
		boot_time[i] = t[i];
	}
	shell_main();
	reboot();
}
