#include "kernel.h"
#include "port.h"

short palfy[18] = {0b11111100, 0b0010100, 0b11100, 
	0, 0b11101000, 0b10101010, 0b11111000,
	0, 0b11111000, 0, 0b11111000, 0b101000, 0b101000,
	0b1000, 0, 0b111000, 0b100000, 0b11111000};

uint32_t boot_time[6];

uint32_t get_boot_date()
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
	asm volatile("hlt");
}

void bsod(const char* msg)
{
	uint32_t color = COLOR_WHITE | COLOR_BLUE << 4;
	terminal_setcolor(color);
	terminal_clear();
	terminal_writestring("A problem has been detected and AvenueOS has been shut down to prevent damage\nto your computer.\n\n");
	terminal_writestring(msg); terminal_writestring("\n\nIf this is the first time you've seen this Stop error screen,\nrestart your computer and, If this screen appears again, follow\n");
	terminal_writestring("these steps:\n\n1. Pronounce the Confession of the Faith:\n\tThere is no system but GNU, and Linux is one of its kernels.\n2. delete system32\n3. Never return.\n\n");
}

void reg_dump(registers_t regs)
{
	terminal_writestring("DS="); terminal_writeint(regs.ds); terminal_putchar(' ');
	terminal_writestring("EDI="); terminal_writeint(regs.edi); terminal_putchar(' ');
	terminal_writestring("ESI="); terminal_writeint(regs.esi); terminal_putchar('\n');
	terminal_writestring("EBP="); terminal_writeint(regs.ebp); terminal_putchar(' ');
	terminal_writestring("ESP="); terminal_writeint(regs.esp); terminal_putchar(' ');
	terminal_writestring("EBX="); terminal_writeint(regs.ebx); terminal_putchar('\n');
	terminal_writestring("EDX="); terminal_writeint(regs.edx); terminal_putchar(' ');
	terminal_writestring("ECX="); terminal_writeint(regs.ecx); terminal_putchar(' ');
	terminal_writestring("EAX="); terminal_writeint(regs.eax); terminal_putchar('\n');
	//eip, cs, eflags, useresp, ss
	terminal_writestring("EIP="); terminal_writeint(regs.eip); terminal_putchar(' ');
	terminal_writestring("CS="); terminal_writeint(regs.cs); terminal_putchar(' ');
	terminal_writestring("EFLAGS="); terminal_writeint(regs.eflags); terminal_putchar('\n');
	terminal_writestring("USERESP="); terminal_writeint(regs.useresp); terminal_putchar(' ');
	terminal_writestring("SS="); terminal_writeint(regs.ss); terminal_putchar(' ');
	terminal_writestring("Interrupt: "); terminal_writeint(regs.int_no); terminal_putchar('\n');
	terminal_writestring("Error code: "); terminal_writeint(regs.err_code); terminal_putchar('\n');
}

void kpanic(const char* msg, registers_t regs)
{
	bsod(msg);
	reg_dump(regs);
	halt();
}

void reboot()
{
	terminal_clear();
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
				terminal_putentryat('#', color, i, j + oy);
		}
	}
	terminal_setcursor(0, 9 + oy);
}

void oscall_handler(registers_t regs)
{
	terminal_writestring("System Call happened\n");
}

void kernel_main(struct multiboot_header *mboot_ptr)
{
	terminal_initialize();
	init_descriptor_tables();
	//terminal_clear();
	asm volatile("sti");
	init_timer(1000); //1000 Hz
	keyb_init();
	read_rtc();
	init_mouse();
	vfs_init();
	if(mboot_ptr->magic == MULTIBOOT_HEADER_MAGIC)
	{
		terminal_prfxi(ticks(), "GRUB Multiboot OK\n");
	}
	//logo();
	terminal_writestring(KERNEL_NAME);
	terminal_writestring(KERNEL_VERSION); terminal_writestring("\n");
	uint32_t* t = get_time();
	for(int i = 0; i < 6; i++)
	{
		boot_time[i] = t[i];
	}
	//asm("int $0x80");
	shell_main();
	reboot();
}