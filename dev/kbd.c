#include <dev/kbd.h>
#include <string.h>
#include <stdint.h>
#include <dtables.h>
#include <stdio.h>
#include <port.h>

bool* signals[256];
int keyevents_sp = -1;
bool shift = false;
bool ctrl = false;
bool alt = false;

const char keys[] = {
  0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
  '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u',
  'i', 'o', 'p', '{', '}', '\n', 0, 'a', 's', 'd', 'f',
  'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '|', 'z', 'x',
  'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
  [74] = '-', [78] = '+', [127] = 0
};

const char keys_shift[] = {
  0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
  '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U',
  'I', 'O', 'P', '[', ']', '\n', 0, 'A', 'S', 'D', 'F',
  'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '\'', 'Z', 'X',
  'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ',
  [74] = '-', [78] = '+', [127] = 0
};

static void kbdcallback(registers_t regs)
{
  struct keyevent* e;
  if(!(inb(0x64) & 1)) return;
	uint32_t scancode;
	int a;
	scancode = inb(0x60);
	a = inb(0x61);
	//outb(0x61, a | 0x80);
	//outb(0x61, a);
  bool doublescan = scancode == 0xE0;
  if(doublescan)
  {
    scancode = inb(0x60);
    a = inb(0x61);
    outb(0x61, a | 0x80);
    outb(0x61, a);
  }
  int kc = scancode & ~0x80;
  if(kc == 0x2a || kc == 0x36)
  {
    shift = !(scancode & 0x80);
    goto end;
  }
  if(kc == 0x1d)
  {
    ctrl = !(scancode & 0x80);
    goto end;
  }
  if(kc == 0x38)
  {
    alt = !(scancode & 0x80);
    goto end;
  }
  if(keyevents_sp + 1 <= 255)
  {
    e = &keyevents[++keyevents_sp];
    e->keycode = kc;
    e->release = scancode & 0x80;
    char keychar = 0;
    if(e->keycode < 78) keychar = (shift ? keys_shift: keys)[e->keycode];
    e->character = keychar;
    e->shift = shift;
    e->ctrl = ctrl;
    e->alt = alt;
    if(kc == 0x53 && doublescan && e->alt && e->ctrl)
    {
      extern void kernupd_finish(void);
      kernupd_finish();
    }
  }
end:
  outb(0x20, 0x20); //End of Interrupt
}

struct keyevent* kbdpoll()
{
  //asm("cli");
  if(keyevents_sp < 0)
  {
    keyevents_sp = -1;
    return NULL;
  }
  struct keyevent* ret = &keyevents[keyevents_sp];
  keyevents_sp--;
  //asm("sti");
  return ret;
}

bool kbdavail()
{
  return keyevents_sp > -1;
}

void kbdregsign(bool* b)
{
  for(int i = 0; i < 256; i++)
  {
    if(signals[i] == 0)
    {
      signals[i] = b;
      break;
    }
  }
}

void kbdunregsign(bool* b)
{
  for(int i = 0; i < 256; i++)
  {
    if(signals[i] == b)
    {
      signals[i] = 0;
      break;
    }
  }
}

void sound(uint32_t freq)
{
 uint32_t idiv;
 uint8_t tmp;

 idiv = 1193180 / freq;
 outb(0x43, 0xb6);
 outb(0x42, (uint8_t) (idiv) );
 outb(0x42, (uint8_t) (idiv >> 8));

 tmp = inb(0x61);
   if (tmp != (tmp | 3)) {
   outb(0x61, tmp | 3);
 }
}

void nosound()
{
 uint8_t tmp = inb(0x61) & 0xFC;
 outb(0x61, tmp);
}

void kbdinit()
{
  memset(keyevents, 0, 256*sizeof(struct keyevent));
  memset(signals, 0, 256*sizeof(bool));
  register_interrupt_handler(IRQ1, &kbdcallback);
  puts("Keyboard IRQ callback registered\n");
}
