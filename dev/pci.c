#include <dev/pci.h>
#include <stdint.h>
#include <stddef.h>
#include <port.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dev/pci/ne2k_pci.h>
#include <dev/pci/pcnet3.h>
#include <dev/pci/ehci.h>
#include <dev/pci/virtboxgfx.h>
#include <dev/pci/virtio-net.h>
#include <dev/pci/debugdrv.h>
#include <dev/pci/amd_rv100.h>

struct _pci_device {
  uint16_t vendor;
  uint16_t device;
  char name[64];
  void (*initfunc)(uint8_t, uint8_t);
} _pci_devices[] = {
//  Vendor   Device  Name
    {0x8086, 0x1237, "Intel PCI & Memory", NULL},
    {0x8086, 0x7000, "Intel PIIX3 PCI2ISA Bridge (Triton II)", NULL},
    {0x8086, 0x1038, "Intel PRO/100", NULL},
    {0x8086, 0x100e, "Intel Pro 1000/MT", NULL},
    {0x1234, 0x1111, "QEMU/Bochs Virtual VGA", NULL},
    {0x10ec, 0x8029, "Ne2000 PCI", &ne2k_pciinit},
    {0x11c1, 0x0450, "LSI Winmodem 56k", NULL},
    {0x8086, 0x3576, "Intel Host-AGP Bridge", NULL},
    {0x1022, 0x2000, "PCnet LANCE PCI Ethernet Controller", &pcnet3init},
    {0x8086, 0x7113, "PIIX4/4E/4M Power Management Controller", NULL},
    {0x8086, 0x265c, "USB 2.0 EHCI Controller", &ehciinit},
    {0x80ee, 0xbeef, "Virtualbox Graphics Adapter", &vbgfxinit},
    {0x80ee, 0x7145, "Virtualbox Graphics Adapter", &vbgfxinit},
    {0x1106, 0x3371, "VIA Chrome 9 HC", NULL},
    {0x8086, 0x3575, "Host-Hub I/F Bridge / SDRAM Controller", NULL},
    {0x8086, 0x2482, "USB Controller", NULL},
    {0x8086, 0x2448, "Hub Interface to PCI Bridge", NULL},
    {0x8086, 0x248c, "LPC Interface or ISA bridge", NULL},
    {0x1002, 0x4c59, "ATI Mobility Radeon AGP", &amdrv100init},
    {0x104c, 0xac51, "Texas Instruments 1420 PCI2CB", NULL},
    {0x125d, 0x1988, "ESS Allegro Audio", NULL},
    {0x1af4, 0x1000, "VirtIO Network Card", &virtionetinit},
    {}
};

#define PCI_PORT_CONF_ADDR 0xCF8
#define PCI_PORT_CONF_DATA 0xCFC

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
  uint32_t addr;
  uint32_t lbus = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;
  uint16_t t = 0;

  addr = (uint32_t)((lbus<<16) | (lslot << 11) | (lfunc << 8) |
          (offset & 0xfc) | ((uint32_t)0x80000000));
  outl(PCI_PORT_CONF_ADDR, addr);
  io_wait();
  uint32_t in = inl(0xCFC);
  t = (uint16_t)((in >> ((offset & 2) * 8)) & 0xffff);
  return t;
}

uint32_t pci_config_read_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
  uint32_t addr;
  uint32_t lbus = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;
  uint32_t t = 0;

  addr = (uint32_t)((lbus<<16) | (lslot << 11) | (lfunc << 8) |
          (offset & 0xfc) | ((uint32_t)0x80000000));
  outl(PCI_PORT_CONF_ADDR, addr);
  io_wait();
  uint32_t in = inl(0xCFC);
  t = ((in >> ((offset & 2) * 8)));
  return t;
}

void pci_config_write_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t val)
{
  uint32_t addr;
  uint32_t lbus = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;

  addr = (uint32_t)((lbus<<16) | (lslot << 11) | (lfunc << 8) |
          (offset & 0xfc) | ((uint32_t)0x80000000));
  outl(PCI_PORT_CONF_ADDR, addr);
  io_wait();
  outw(PCI_PORT_CONF_DATA, val);
  io_wait();
}

void pci_config_write_dword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t val)
{
  uint32_t addr;
  uint32_t lbus = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;

  addr = (uint32_t)((lbus<<16) | (lslot << 11) | (lfunc << 8) |
          (offset & 0xfc) | ((uint32_t)0x80000000));
  outl(PCI_PORT_CONF_ADDR, addr);
  io_wait();
  outl(PCI_PORT_CONF_DATA, val);
  io_wait();
}

void pci_config_write_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint8_t val)
{
  uint32_t addr;
  uint32_t lbus = (uint32_t)bus;
  uint32_t lslot = (uint32_t)slot;
  uint32_t lfunc = (uint32_t)func;

  addr = (uint32_t)((lbus<<16) | (lslot << 11) | (lfunc << 8) |
          (offset & 0xfc) | ((uint32_t)0x80000000));
  outl(PCI_PORT_CONF_ADDR, addr);
  io_wait();
  outb(PCI_PORT_CONF_DATA, val);
  io_wait();
}

void pci_ls()
{
  for(int bus = 0; bus < 256; bus++)
  {
    for(int slot = 0; slot < 32; slot++)
    {
      uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
      if(vendor == 0xffff)
      {
        continue;
      }
      uint16_t device = pci_config_read_word(bus, slot, 0, 2);
      printf("  %x:%x - Vendor: 0x%x Device: 0x%x - ", bus, slot, vendor, device);
      struct _pci_device* pdptr = &_pci_devices[0];
      while(pdptr->vendor != 0)
      {
        if(pdptr->vendor == vendor && pdptr->device == device)
        {
          puts(pdptr->name);
          break;
        }
        pdptr++;
      }
      putc('\n');
    }
  }
}

void pciinit()
{
  for(int bus = 0; bus < 256; bus++)
  {
    for(int slot = 0; slot < 32; slot++)
    {
      uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
      if(vendor == 0xffff)
      {
        continue;
      }
      uint16_t device = pci_config_read_word(bus, slot, 0, 2);
      struct _pci_device* pdptr = &_pci_devices[0];
      while(pdptr->vendor != 0)
      {
        if(pdptr->vendor == vendor && pdptr->device == device)
        {
          if(pdptr->initfunc != NULL) pdptr->initfunc(bus, slot);
          break;
        }
        pdptr++;
      }
    }
  }
}
