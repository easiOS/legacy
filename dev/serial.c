/* EasiOS kernel.c
 * ----------------
 * Author(s): Daniel (Easimer) Meszaros
 * ----------------
 * Description: serial ports
 */

#include <dev/serial.h>
#include <net/udp.h>
#include <string.h>

struct serial_port_table {
  uint16_t port;
  bool enabled;
  uint8_t mode; //0 - text, 1 - slip
} ports[4];

void serinit()
{
  memset(&ports, 0, sizeof(struct serial_port_table) * 4);
}

void serinitport(uint16_t port)
{
  for(int i = 0; i < 4; i++)
  {
    if(ports[i].port == port && ports[i].enabled) return;
    if(ports[i].port == 0 && !ports[i].enabled)
    {
      ports[i].port = port;
      ports[i].enabled = true;
      break;
    }
  }
  outb(port + 1, 0x00);    // Disable all interrupts
  outb(port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
  outb(port + 0, 0x01);    // Set divisor to 1 (lo byte) 115200 baud
  outb(port + 1, 0x00);    //                  (hi byte)
  outb(port + 3, 0x03);    // 8 bits, no parity, one stop bit
  outb(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
  outb(port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
}

void sersetmode(uint16_t port, uint8_t mode)
{
  for(int i = 0; i < 4; i++)
  {
    if(ports[i].port == port)
    {
      ports[i].mode = mode;
      return;
    }
  }
}

void serwrite(uint16_t port, char a)
{
  uint8_t textmode = 1;
  for(int i = 0; i < 4; i++)
  {
    if(ports[i].port == port && ports[i].mode != 0)
    {
      textmode = 0;
      break;
    }
  }
  while (!sertxempty(port));

  if(a == '\n' && textmode)
  {
    outb(port, '\r');
    outb(port, '\n');
  }
  else
    outb(port, a);
  //io_wait();
}

bool sertxempty(uint16_t port)
{
  return inb(port + 5) & 0x20;
}

char serread(uint16_t port)
{
  while (!serrxavail(port));
  return inb(port);
}

bool serenabled(uint16_t port)
{
  for(int i = 0; i < 4; i++)
  {
    if(ports[i].port == port)
    {
      return ports[i].enabled;
    }
  }
  return false;
}

bool serrxavail(uint16_t port)
{
  return inb(port + 5) & 1;
}

void serswrite(uint16_t port, const char* str)
{
  for(int i = 0; i < 4; i++)
  {
    if(ports[i].port == port && ports[i].mode == 1)
    {
      /*udp_header h;
      udp_create(&h, 8, 0, (char*)str, strlen(str) + 1);
      for(int i = 0; i < 8; i++)
      {
        serwrite(port, ((char*)&h)[i]);
      }
      serwrite(port, 0xE0); serwrite(port, 0x55); //protocol identifier
      while(*str != '\0')
      {
        serwrite(port, *str++);
      }
      return;*/
    }
  }
  int i = 0;
  while(str[i] != '\0')
  {
    serwrite(port, str[i]);
    i++;
  }
}
