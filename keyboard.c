#include <stddef.h>
#include <stdint.h>
#include "port.h"
#include "dtables.h"
#include "keyboard.h"
#include "video.h"
#include "timer.h"

const char keyb_dict[] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',   /* 9 */
  '9', '0', '-', '=', '\b',   /* Backspace */
  '\t',         /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',   /* Enter key */
    0,         /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',   /* 39 */
'\'', '`',   0,      /* Left shift */
'\\', 'z', 'x', 'c', 'v', 'b', 'n',         /* 49 */
  'm', ',', '.', '/',   0,            /* Right shift */
  '*',
    0,   /* Alt */
  ' ',   /* Space bar */
    0,   /* Caps lock */
    0,   /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,   /* < ... F10 */
    0,   /* 69 - Num lock*/
    0,   /* Scroll Lock */
    0,   /* Home key */
    0,   /* Up Arrow */
    0,   /* Page Up */
  '-',
    0,   /* Left Arrow */
    0,
    0,   /* Right Arrow */
  '+',
    0,   /* 79 - End key*/
    0,   /* Down Arrow */
    0,   /* Page Down */
    0,   /* Insert Key */
    0,   /* Delete Key */
    0,   0,   0,
    0,   /* F11 Key */
    0,   /* F12 Key */
    0,   /* All other keys are undefined */
};

//bool keys_pressed[256];
uint32_t ckey;
bool avail = false;

static void keyb_callback(registers_t regs)
{
  if(!(inb(0x64) & 1)) return;
	uint32_t scancode;
	int a;
	scancode = inb(0x60);
	a = inb(0x61);
	outb(0x61, a | 0x80);
	outb(0x61, a);
  if(scancode == 0xE0)
  {
    scancode = inb(0x60);
    a = inb(0x61);
    outb(0x61, a | 0x80);
    outb(0x61, a);
    ckey = scancode;
  }
  else
  {
    ckey = scancode;
  }
  avail = true;
  outb(0x20, 0x20); //End of Interrupt
}

bool keyb_isavail()
{
  return avail;
}

uint32_t keyb_get()
{
  avail = false;
  uint32_t ck = ckey;
  ckey = 0;
  return ck;
}

char keyb_s2c(uint32_t scancode)
{
	return keyb_dict[scancode];
}

void keyb_init()
{
  //it looks like grub sets up PS/2 for us, so we just need to register the
  //callback
  terminal_prfxi(ticks(), "Keyboard IRQ callback registered\n");
  register_interrupt_handler(IRQ1, &keyb_callback);
}

/*void keyb_init()
{
  terminal_writestring("Keyboard PS2 test...");
  unsigned int start = ticks();
  ps2test();

  terminal_writestring("OK!\n");
  outb(0x64, 0xAE);
  terminal_writestring("First PS/2 port enabled\n");
  terminal_prfxi(ticks(), "Keyboard IRQ callback registered\n");
  register_interrupt_handler(IRQ1, &keyb_callback);
  terminal_writestring("Clearing keyboard buffers");
  while(inb(0x64) & 1)
  {
    terminal_putchar('.');
    inb(0x60);
    sleep(5);
  }
  terminal_putchar('\n');
}*/

void keyb_clr()
{
  ckey = 0;
  avail = false;
}

void keyb_pak() //press any key
{
  terminal_writestring("Press any key to continue...\n");
  keyb_clr();
  while(!avail);
  keyb_clr();
}

bool ps2test()
{
  return true;
}
