#include <dev/pci/ohci.h>
#include <dev/pci.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t* ohci_read_hcopreg(uint32_t iobase, uint8_t off)
{
  return ((uint32_t*)(iobase + off));
}

void ohciinit(uint8_t bus, uint8_t slot)
{
  uint32_t iobase;
  char b[64];
  puts("ohci: initializing on PCI bus:slot "); itoa(bus, b, 10);
  puts(b); putc(':'); itoa(slot, b, 10); puts(b); putc('\n');
  uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
  if(vendor == 0xFFFF)
  {
    puts("ohci: init failed: device not found on specified bus.slot\n");
    return;
  }
  if(0)
  {
    puts("ohci: init failed: device is not compatible: ");
    return;
  }
  iobase = pci_config_read_dword(bus, slot, 0, 0x10);
  if(iobase & 1)
  {
    iobase &= 0xFFFFFFFC;
    puts("ohci: com thru I/O port\n");
  }
  else
  {
    iobase &= 0xFFFFFFF0;
    puts("ohci: com thru MMIO\n");
  }
  puts("ohci: iobase: "); itoa(iobase, b, 16); puts(b); putc('\n');
  uint32_t* revision = ohci_read_hcopreg(iobase, OHCI_REG_HCREVI);
  printf("ohci: Revision: %s\n", (*revision) == 0x10 ? "1.0" : "1.1");
  puts("ohci: resetting\n");
  struct ohci_reg_cmdstatus* cmds = (struct ohci_reg_cmdstatus*)ohci_read_hcopreg(iobase, OHCI_REG_HCCMDS);
  cmds->hc_reset = 1;
  puts("ohci: put device into operational state\n");
  struct ohci_reg_ctrl* ctrl = (struct ohci_reg_ctrl*)ohci_read_hcopreg(iobase, OHCI_REG_HCCTRL);
  printf("ochi: status: %x\n", ctrl->functional_state);
  ctrl->functional_state = 0b10;
  printf("ochi: status: %x\n", ctrl->functional_state);
}
