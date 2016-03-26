#include <drivers/kbd.h>
#include <dtables.h>

char* keylayout = us_keys;
char* keylayout_shift = us_keys_shift;

struct input_event ioevents[256];
int ioev_in, ioev_out;
int ioflags;

DTABLES_HNDLR kbdhandler(registers_t regs)
{
	if(ioev_in == ((ioev_out - 258) % 257))
		return; //queue is full
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
			struct input_event* e = ioevents[ioev_in];

			e->keycode = kc;
			e->flags = 0;
			e->flags |= (ioflags & 0b11110000) | ((doublescan) << IOFLAGS_DS);
			if(kc < 78)
				e->keychar = (shift ? keylayout_shift : keylayout)[kc];

			ioev_in = (ioev_in + 1) % 257;
			break;
		}
	}
}

struct input_event* kbdpoll(void)
{
	if(ioev_in == ioev_out)
		return NULL; //queue is empty
	struct input_event* e = ioevents[ioev_out];
	ioev_out = (ioev_out + 1) % 257;
	return e;
}

int kbdavaialble(void)
{
	return ioev_in != ioev_out;
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
	memset(ioevents, 0, 256 * (struct input_event));
	ioev_in = ioev_out = 0;
	ioflags = 0;
	dtables_reg(IRQ1, &kbdhandler);
}

int kbdavailable();
struct input_event* kbdpoll();