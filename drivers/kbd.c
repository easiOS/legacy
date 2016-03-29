#include <drivers/kbd.h>
#include <dtables.h>
#include <ioevents.h>
#include <port.h>

char us_keys[] = {
  0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
  '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u',
  'i', 'o', 'p', '{', '}', '\n', 0, 'a', 's', 'd', 'f',
  'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '|', 'z', 'x',
  'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
  [74] = '-', [78] = '+', [127] = 0
};

char us_keys_shift[] = {
  0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
  '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U',
  'I', 'O', 'P', '[', ']', '\n', 0, 'A', 'S', 'D', 'F',
  'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '\'', 'Z', 'X',
  'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ',
  [74] = '-', [78] = '+', [127] = 0
};

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