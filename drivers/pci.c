#include <drivers/pci.h>
#include <port.h>

struct _pci_dev {
  unsigned short vendor, device;
  char name[128];
  void (*initfunc)(unsigned char, unsigned char, unsigned char);
} pci_devices[] = {

};

void pciinit(void)
{
	for(int bus = 0; bus < 256; bus++)
		for(int device = 0; device < 32; device++)
		{
			if(PCIC_READ_VENDOR(bus, device, 0) == 0xFFFF)
				continue;
			unsigned char function = 0;
			struct _pci_dev* pd = pci_devices;
			unsigned char headertype = PCIC_READ_HDRT(bus, device, function);
			if((headertype & 0x80) != 0)
			{
				for(function = 1; function < 8; function++)
					if(PCIC_READ_VENDOR(bus, device, function) != 0xFFFF)
					{

					}
			}
		}
}

unsigned short pci_cfg_read_w(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset)
{
  unsigned addr;
  unsigned lbus = (unsigned)bus;
  unsigned lslot = (unsigned)slot;
  unsigned lfunc = (unsigned)func;
  unsigned short t = 0;

  addr = (unsigned)((lbus<<16) | (lslot << 11) | (lfunc << 8) |
          (offset & 0xfc) | ((unsigned)0x80000000));
  outl(PCI_PORT_CONF_ADDR, addr);
  io_wait();
  unsigned in = inl(0xCFC);
  t = (unsigned short)((in >> ((offset & 2) * 8)) & 0xffff);
  return t;
}

unsigned pci_cfg_read_dw(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset)
{
  unsigned addr;
  unsigned lbus = (unsigned)bus;
  unsigned lslot = (unsigned)slot;
  unsigned lfunc = (unsigned)func;
  unsigned t = 0;

  addr = (unsigned)((lbus<<16) | (lslot << 11) | (lfunc << 8) |
          (offset & 0xfc) | ((unsigned)0x80000000));
  outl(PCI_PORT_CONF_ADDR, addr);
  io_wait();
  unsigned in = inl(0xCFC);
  t = ((in >> ((offset & 2) * 8)));
  return t;
}

void pci_cfg_write_w(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset, unsigned short val)
{
  unsigned addr;
  unsigned lbus = (unsigned)bus;
  unsigned lslot = (unsigned)slot;
  unsigned lfunc = (unsigned)func;

  addr = (unsigned)((lbus<<16) | (lslot << 11) | (lfunc << 8) |
          (offset & 0xfc) | ((unsigned)0x80000000));
  outl(PCI_PORT_CONF_ADDR, addr);
  io_wait();
  outw(PCI_PORT_CONF_DATA, val);
  io_wait();
}

void pci_cfg_write_dw(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset, unsigned val)
{
  unsigned addr;
  unsigned lbus = (unsigned)bus;
  unsigned lslot = (unsigned)slot;
  unsigned lfunc = (unsigned)func;

  addr = (unsigned)((lbus<<16) | (lslot << 11) | (lfunc << 8) |
          (offset & 0xfc) | ((unsigned)0x80000000));
  outl(PCI_PORT_CONF_ADDR, addr);
  io_wait();
  outl(PCI_PORT_CONF_DATA, val);
  io_wait();
}

void pci_cfg_write_b(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset, unsigned char val)
{
  unsigned addr;
  unsigned lbus = (unsigned)bus;
  unsigned lslot = (unsigned)slot;
  unsigned lfunc = (unsigned)func;

  addr = (unsigned)((lbus<<16) | (lslot << 11) | (lfunc << 8) |
          (offset & 0xfc) | ((unsigned)0x80000000));
  outl(PCI_PORT_CONF_ADDR, addr);
  io_wait();
  outb(PCI_PORT_CONF_DATA, val);
  io_wait();
}

unsigned char pci_cfg_read_b(unsigned char bus, unsigned char slot, unsigned char func, unsigned char offset)
{
  unsigned addr;
  unsigned lbus = (unsigned)bus;
  unsigned lslot = (unsigned)slot;
  unsigned lfunc = (unsigned)func;
  unsigned short t = 0;

  addr = (unsigned)((lbus<<16) | (lslot << 11) | (lfunc << 8) |
          (offset & 0xfc) | ((unsigned)0x80000000));
  outl(PCI_PORT_CONF_ADDR, addr);
  io_wait();
  unsigned in = inl(0xCFC);
  t = (unsigned short)((in >> ((offset & 2) * 8)) & 0xff);
  return t;
}