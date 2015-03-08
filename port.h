inline unsigned char inb(unsigned int port)
{
	unsigned char ret;
	asm volatile ("inb %%dx,%%al":"=a" (ret):"d" (port));
	return ret;
}

inline void outb(unsigned int port, unsigned char value)
{
	asm volatile ("outb %%al,%%dx": :"d" (port), "a" (value));
}

inline unsigned short inw(unsigned int port)
{
   unsigned short ret;
   asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));
   return ret;
}