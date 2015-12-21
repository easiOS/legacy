/* EasiOS kernel.c
 * ----------------
 * Author(s): Daniel (Easimer) Meszaros
 * ----------------
 * Description: serial ports
 */

#include <dev/serial.h>

struct serial_port_table {
  uint16_t port;
  bool enabled;
} ports[4];

void serinit()
{
  //memset(&ports, 0, sizeof(struct serial_port_table) * 4);
  uint8_t* ports_8 = (uint8_t*)ports;
  for(int i = 0; i < sizeof(struct serial_port_table) * 4; i++)
  {
    ports_8[i] = 0;
  }
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
  outb(port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
  outb(port + 1, 0x00);    //                  (hi byte)
  outb(port + 3, 0x03);    // 8 bits, no parity, one stop bit
  outb(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
  outb(port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
  serswrite(port, "EasiOS Read-only serial\n");
}

void serwrite(uint16_t port, char a)
{
  while (!sertxempty(port));

  if(a == '\n')
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
  int i = 0;
  while(str[i] != '\0')
  {
    serwrite(port, str[i]);
    i++;
  }
}
