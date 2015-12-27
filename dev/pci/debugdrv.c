//EasiOS PCI Debug Driver
//Loadable at runtime from kshell or eelphant cmdbar
//Prints info like registers, base addresses, etc.
#include <dev/pci/debugdrv.h>
#include <dev/pci.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void pciddrvinit(uint8_t bus, uint8_t slot)
{
  char buffer[64];
  printf("pciddrv: init on bus:slot %x:%x\n", bus, slot);
  uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
  if(vendor == 0xFFFF)
  {
    puts("pciddrv: init failed: device not found on specified bus:slot\n");
    return;
  }
  uint16_t device = pci_config_read_word(bus, slot, 0, 2);
  printf("pciddrv@%x:%x: vendor: 0x%x, device: 0x%x\n", bus, slot, vendor, device);
  uint16_t status = pci_config_read_word(bus, slot, 0, 6);
  uint16_t command = pci_config_read_word(bus, slot, 0, 4);
  printf("pciddrv@%x:%x: status: %s command: %s\n", bus, slot, itoa(status, buffer, 2), itoa(command, buffer, 2));
  uint8_t revid = pci_config_read_word(bus, slot, 0, 8) & 0xFF;
  uint8_t progif = pci_config_read_word(bus, slot, 0, 9) & 0xFF;
  printf("pciddrv@%x:%x: revid: %x progif: %x\n", bus, slot, revid, progif);
  uint8_t class = pci_config_read_word(bus, slot, 0, 0xa) & 0xFF;
  uint8_t subclass = pci_config_read_word(bus, slot, 0, 0xb) & 0xFF;
  printf("pciddrv@%x:%x: class: %x subclass: %x\n", bus, slot, class, subclass);
  uint32_t bar0, bar1, bar2, bar3, bar4, bar5;
  bar0 = pci_config_read_dword(bus, slot, 0, 0x10);
  bar1 = pci_config_read_dword(bus, slot, 0, 0x14);
  bar2 = pci_config_read_dword(bus, slot, 0, 0x18);
  bar3 = pci_config_read_dword(bus, slot, 0, 0x1C);
  bar4 = pci_config_read_dword(bus, slot, 0, 0x20);
  bar5 = pci_config_read_dword(bus, slot, 0, 0x24);
  printf("pciddrv@%x:%x: BAR0-5: 0x%x\n0x%x\n0x%x\n0x%x\n0x%x\n0x%x\n", bus, slot, bar0, bar1, bar2, bar3, bar4, bar5);
  printf("pciddrv@%x:%x: finished.\n", bus, slot);
}
