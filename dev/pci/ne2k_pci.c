#include <dev/pci/ne2k_pci.h>
#include <dev/pci.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void ne2k_pciinit(uint8_t bus, uint8_t slot)
{
  char b[64];
  puts("Initializing ne2k on PCI bus:slot "); itoa(bus, b, 10);
  puts(b); putc(':'); itoa(slot, b, 10); puts(b); putc('\n');
  uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
  if(vendor == 0xFFFF)
  {
    puts("ne2k_pci: init failed: device not found on specified bus.slot\n");
    return;
  }
  uint16_t device = pci_config_read_word(bus, slot, 0, 2);
  if(vendor != 0x10ec || device != 0x8029)
  {
    puts("ne2k_pci: init failed: device is not compatible\n");
    return;
  }
  puts("ne2k_pci: initialized\n");
}
