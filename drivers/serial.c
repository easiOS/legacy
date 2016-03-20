#include <drivers/serial.h>
#include <port.h>

struct s_port {
  unsigned short port;
  int enabled;
  unsigned char divisor;
  unsigned char flags;
} ports[4];

void sinit(void)
{
	memset(&ports, 0, sizeof(struct s_port) * 4);
}

int sinitport(unsigned short port)
{
	for(int i = 0; i < 4; i++)
	{
		if(ports[i].port == port && ports[i].enabled) return -1;
		if(!ports[i].enabled)
		{
			ports[i].port = port;
			ports[i].enabled = 1;
			ports[i].divisor = 1;
			ports[i].flags = 0x03;
			break;
		}
	}
	outb(port + 1, 0x00); //Disable interrupts
	outb(port + 3, 0x80); //Enable DLAB 
	outb(port + 0, 0x01); //Set divisor to 1 (115200 baud) (lo byte)
	outb(port + 1, 0x00); //                               (hi byte)
	outb(port + 3, 0x03); //8 bits, no parity, one stop bit
	outb(port + 2, 0xC7); //Enable FIFO, clear them, 14-byte threshold
	outb(port + 4, 0x0B); //Enable interrupts, set RTS/DSR
	return 0;
}

int sempty(unsigned short port)
{
	return inb(port + 5) & 0x20;
}

int savailable(unsigned short port)
{
	return inb(port + 5) & 1;
}

char sread(unsigned short port)
{
	while(!savailable(port));
	return inb(port);
}

void swrite(unsigned short port, char c)
{
	while(!sempty(port));
	if(c == '\n')
	{
		outb(port, '\r');
		outb(port, '\n');
	}
	else
		outb(port, c);
}

int senabled(unsigned short port)
{
	for(int i = 0; i < 4; i++)
		if(ports[i].port == port)
			return ports[i].enabled;
	return 0;
}

void swrites(unsigned short port, const char* str)
{
	while(*str != 0)
		swrite(port, *str++);
}