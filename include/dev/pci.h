#ifndef H_PCI
#define H_PCI

#include <stdint.h>

void pciinit();
uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
void pci_config_write_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint16_t val);
void pci_config_write_byte(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint8_t val);

#endif
