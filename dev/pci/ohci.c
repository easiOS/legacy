#include <dev/pci/ehci.h>
#include <dev/pci.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EHCI_USBCMD 0x00
#define EHCI_USBSTS 0x04
#define EHCI_USBINTR 0x08
#define EHCI_FRINDEX 0x0c
#define EHCI_CTRLDSSSEGMENT 0x10
#define EHCI_PERIODICLISTTABLE 0x14
#define EHCI_ASYNCLISTADDR 0x18
#define EHCI_CONFIGFLAG 0x40

void ehciinit(uint8_t bus, uint8_t slot)
{
  uint32_t iobase;
  char b[64];
  puts("ehci: initializing on PCI bus:slot "); itoa(bus, b, 10);
  puts(b); putc(':'); itoa(slot, b, 10); puts(b); putc('\n');
  uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
  if(vendor == 0xFFFF)
  {
    puts("ehci: init failed: device not found on specified bus.slot\n");
    return;
  }
  //uint16_t device = pci_config_read_word(bus, slot, 0, 2);
  uint16_t class = pci_config_read_word(bus, slot, 0, 0x08) >> 16;
  uint16_t sclass = pci_config_read_word(bus, slot, 0, 0x09) >> 16;
  uint16_t progif = pci_config_read_word(bus, slot, 0, 0x0A) >> 16;
  //if(class != 0xC || sclass != 0x3 || progif != 0x20)
  if(0)
  {
    puts("ehci: init failed: device is not compatible: ");
    itoa(class, b, 16);
    puts(b); putc(' ');
    itoa(sclass, b, 16);
    puts(b); putc(' ');
    itoa(progif, b, 16);
    puts(b); putc(' ');
    putc('\n');
    return;
  }
  iobase = pci_config_read_word(bus, slot, 0, 0x10) & 0xFFFFFFFC;
  puts("ehci: iobase: "); itoa(iobase, b, 16); puts(b); putc('\n');
  puts("echi: okay\n");
}
