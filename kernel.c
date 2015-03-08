#define KERNEL_NAME "EasiOS - PalfyMuhely\n"
#define KERNEL_VERSION "0.0.3"

#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>

#include "stdmem.h"
#include "video.h"
#include "port.h"
#include "dtables.h"
#include "timer.h"
#include "keyboard.h"
#include "timer.h"
#include "shell.h"

#define ACK 0xFA
#define RES 0xFE

#define KEYB_DAT 0x60
#define KEYB_CMD 0x64

short palfy[18] = {0b11111100, 0b0010100, 0b11100, 
	0, 0b11101000, 0b10101011, 0b11111000,
	0, 0b11111000, 0, 0b11111000, 0b101000, 0b101000,
	0b1000, 0, 0b111000, 0b100000, 0b11111000};

bool get_nth_binary(short binary, int nth)
{
	return (bool)((binary >> nth) & 1);
}

void reboot()
{
	terminal_clear();
	terminal_writestring("The system is going down for reboot NOW!\n");
	sleep(1000);
    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
    asm volatile("hlt");
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

void kernel_main()
{
	terminal_initialize();
	init_descriptor_tables();
	//terminal_clear();
	init_timer(1000);
	keyb_init();
	logo();
	terminal_writestring(KERNEL_NAME);
	terminal_writestring(KERNEL_VERSION); terminal_writestring("\n");
	asm volatile("sti");
	shell_main();
	reboot();
}