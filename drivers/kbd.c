#include <drivers/kbd.h>
#include <dtables.h>
#include <ioevents.h>
#include <port.h>

char* keylayout = us_keys;
char* keylayout_shift = us_keys_shift;

int ioflags;

DTABLES_HNDLR kbdhandler(registers_t regs)
{
	if(!(inb(0x64) & 1))
		return;

	unsigned int scancode;
	int a, doublescan;
	scancode = inb(0x60);
	a = inb(0x61);
	doublescan = scancode == 0xE0;
	if(doublescan)
	{
		scancode = inb(0x60);
		a = inb(0x61);
		outb(0x61, a | 0x80);
		outb(0x61, a);
	}

	int kc = scancode & ~0x80;
	switch(kc)
	{
		case 0x2a:
		case 0x36:
		{
			ioflags = !(scancode & 0x80) << IOFLAGS_SHIFT;
			break;
		}
		case 0x1d:
		{
			ioflags = !(scancode & 0x80) << IOFLAGS_CTRL;
			break;
		}
		case 0x38:
		{
			ioflags = !(scancode & 0x80) << IOFLAGS_ALT;
			break;
		}
		default:
		{
			struct ioevent ioe;
			struct input_event* e = &ioe.event.userinput;

			e->keycode = kc;
			e->flags = 0;
			e->flags |= (ioflags & 0b11110000) | ((doublescan) << IOFLAGS_DS);
			if(kc < 78)
				e->character = (ioflags & IOFLAGS_SHIFT ? keylayout_shift : keylayout)[kc];

			ioe_push(&ioe);
			break;
		}
	}
}

void kbdsetlayout(char* layout, char* layout_shift)
{
	keylayout = layout;
	keylayout_shift = layout_shift;
}

void kbdresetlayout(void)
{
	keylayout = us_keys;
	keylayout_shift = us_keys_shift;
}

void kbdinit()
{
	ioflags = 0;
	dtables_reg(IRQ1, &kbdhandler);
}