#include "ps2.h"
#include "video.h"
#include "port.h"

//IO ports
//0x20  R     Controller Configuration Byte
//0x60    W   Controller Configuration Byte
//0x60  R W   Data Port, Command Result Port
//0x64  R     Status Register
//0x64    W   Command Register

void init_ps2(void)
{
  terminal_writestring("Initialize PS/2 Controller\n");
  //disable devices
  outb(0x64, 0xAD);
  io_wait();
  terminal_writestring("Disabling devices...");
  outb(0x64, 0xA7);
  io_wait();
  terminal_putchar('\n');
  terminal_writestring("Clearing buffers...");
  for(int i = 0; i < 100; i++)
  {
    inb(0x60);
  }
  terminal_putchar('\n');
  terminal_writestring("Reading config...\n");
  io_wait();
  uint8_t ps2_cfg = inb(0x20);
  uint8_t ps2_ncfg = ps2_cfg & 0b10111100; //clear bits 0, 1 and 6
  io_wait();
  outb(0x64, 0xAA);
  io_wait();
  uint8_t ps2_st_res = inb(0x60);
  if(ps2_st_res == 0x55)
  {
    terminal_writestring("PS/2 Controller self-test OK\n");
  }
  else
  {
    terminal_writestring("PS/2 Controller self-test FAIL (");
    terminal_writeint(ps2_st_res);
    terminal_writestring(")\n");
    return;
  }
  terminal_writestring("Testing first port...");
  outb(0x64, 0xAB);
  io_wait();
  if(!inb(0x60))
  {
    terminal_writestring("OK...");
    outb(0x64, 0xAE);
    io_wait();
    terminal_writestring("enabled!\n");
    ps2_ncfg |= 1;
  }
  else
  {
    terminal_writestring("FAIL\n");
    return;
  }
  terminal_writestring("Writing config...\n");
  outb(0x60, ps2_ncfg);
  terminal_writestring("Resetting PS/2 devices...");
  uint8_t reset = 0xFE;
  int reset_counter = 0;
  while(reset == 0xFE)
  {
    outb(0x64, 0xFF);
    reset = inb(0x60);
    switch(reset)
    {
      case 0xAA:
        terminal_writestring("self-test OK\n");
        break;
      case 0xFC:
      case 0xFD:
        terminal_writestring("self-test FAIL\n");
        return;
        break;
      case 0xFE:
        terminal_putchar('.');
        reset_counter++;
        break;
    }
    if(reset_counter > 100)
    {
      terminal_writestring("I/O FAIL\n");
      return;
    }
  }
  terminal_writestring("PS/2 init OK!\n");
}
