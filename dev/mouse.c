#include <mouse.h>
#include <port.h>
#include <stdint.h>
#include <dtables.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

uint8_t mouse_cycle=0;
int8_t mouse_byte[3];
int8_t mouse_x=0;
int8_t mouse_y=0;

int mouseevents_sp = -1;

void mousecallback(registers_t regs)
{
  struct mouseevent* e;
  switch(mouse_cycle)
  {
    case 0:
      mouse_byte[0]=inb(0x60);
      mouse_cycle++;
      break;
    case 1:
      mouse_byte[1]=inb(0x60);
      mouse_cycle++;
      break;
    case 2:
      mouse_byte[2]=inb(0x60);
      mouse_y += (mouse_byte[0] >> 4 & 1 ? 1 : -1) * mouse_byte[1] / 8;
      mouse_x += (mouse_byte[0] >> 5 & 1 ? -1 : 1) * mouse_byte[2] * 4;
      mouse_cycle=0;
      if(mouseevents_sp + 1 > 255) break;
      e = &mouseevents[++mouseevents_sp];
      e->dy = (mouse_byte[0] >> 4 & 1 ? 1 : -1) * mouse_byte[1] / 8;
      e->dx = (mouse_byte[0] >> 5 & 1 ? -1 : 1) * mouse_byte[2] * 4;
      e->left = mouse_byte[0] & 1;
      e->right = mouse_byte[0] >> 1 & 1;
      e->middle = mouse_byte[0] >> 2 & 1;
      break;
  }
}

inline void mousewait(uint8_t a_type)
{
  unsigned int _time_out=100000;
  if(a_type==0)
  {
    while(_time_out--)
    {
      if((inb(0x64) & 1)==1)
      {
        return;
      }
    }
    return;
  }
  else
  {
    while(_time_out--)
    {
      if((inb(0x64) & 2)==0)
      {
        return;
      }
    }
    return;
  }
}

inline void mousewrite(uint8_t a_write)
{
  mousewait(1);
  outb(0x64, 0xD4);
  mousewait(1);
  outb(0x60, a_write);
}

uint8_t mouseread()
{
  mousewait(0);
  return inb(0x60);
}

bool mouseavail()
{
  return mouseevents_sp > -1;
}

struct mouseevent* mousepoll()
{
  asm("cli");
  if(mouseevents_sp < 0)
  {
    mouseevents_sp = -1;
    return NULL;
  }
  struct mouseevent* ret = &mouseevents[mouseevents_sp];
  mouseevents_sp--;
  asm("sti");
  return ret;
}

void mousegetcoords(int32_t* x, int32_t* y)
{
  *x = mouse_x;
  *y = mouse_y;
}

void mouseinit()
{
  memset(mouseevents, 0, 256*sizeof(struct mouseevent));
  uint8_t _status;

  //enable the auxiliary mouse device
  mousewait(1);
  outb(0x64, 0xA8);

  //enable interrupts
  mousewait(1);
  outb(0x64, 0x20);
  mousewait(0);
  _status=(inb(0x60) | 2);
  mousewait(1);
  outb(0x64, 0x60);
  mousewait(1);
  outb(0x60, _status);

  //use default mouse settings
  mousewrite(0xF6);
  mouseread();

  mousewrite(0xF4);
  mouseread();
  puts("Mouse initialized\n");
  register_interrupt_handler(IRQ12, mousecallback);
}
