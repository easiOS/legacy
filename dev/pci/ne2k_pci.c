#include <dev/pci/ne2k_pci.h>
#include <dev/pci.h>
#include <dev/timer.h>
#include <port.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NE_CMD	 	0x00
#define NE_DATAPORT	0x10	/* NatSemi-defined port window offset. */
#define NE_RESET	0x1f	/* Issue a read to reset, a write to clear. */
#define NE_IO_EXTENT	0x20

#define NESM_START_PG	0x40	/* First page of TX buffer */
#define NESM_STOP_PG	0x80	/* Last page +1 of RX ring */

void ne2k_pciinit(uint8_t bus, uint8_t slot)
{
  uint32_t iobase;
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
  uint32_t bar0;
  bar0 = (((uint32_t)pci_config_read_word(bus, slot, 0, 0x10)) << 16) | (pci_config_read_word(bus, slot, 0, 0x12) & 0xFFFF);
  //bar1 = pci_config_read_word(bus, slot, 0, 0x14) << 16 | pci_config_read_word(bus, slot, 0, 0x16);
  itoa(bar0, b, 16);
  puts("ne2k_pci: BAR0 address: ");
  puts(b);
  putc('\n');
  iobase = ((bar0 >> 16) & 0xFFFFFFFC);
  puts("ne2k_pci: IO base at ");
  itoa(iobase, b, 16);
  puts(b); putc('\n');
  int reg0 = inb(iobase);
  if(reg0 == 0xFF)
  {
    puts("ne2k_pci: what teh hell\n");
    return;
  }
  //8390 check

  int regd;
  outb(iobase, 0x20+0x40+0x01);
  regd = inb(iobase + 0x0d);
  outb(iobase + 0x0d, 0xff);
  outb(iobase, 0x20);
  inb(iobase + 0x0d);
  if (inb(iobase + 0x0d) != 0)
  {
    outb(iobase, reg0);
    outb(iobase + 0x0d, regd);
    return;
  }

  puts("ne2k_pci: Resetting NIC..");
  outb(iobase + 0x1F, inb(iobase + 0x1F));
  putc('.');
  int reset_start = ticks();
  while((inb(iobase + 0x07) & 0x80) == 0)
  {
    if(ticks() - reset_start > 2)
    {
      puts("\nne2k_pci: Card failure (no reset ack)\n");
      return;
      break;
    }
  }
  puts("done!\n");
  outb(iobase + 0x07, 0xFF);
  uint8_t prom[32];
  outb(iobase, (1 << 5) | 1);	// page 0, no DMA, stop
  outb(iobase + 0x0E, 0x49);		// set word-wide access
  outb(iobase + 0x0A, 0);		// clear the count regs
  outb(iobase + 0x0B, 0);
  outb(iobase + 0x0F, 0);		// mask completion IRQ
  outb(iobase + 0x07, 0xFF);
  outb(iobase + 0x0C, 0x20);		// set to monitor
  outb(iobase + 0x0D, 0x02);		// and loopback mode.
  outb(iobase + 0x0A, 32);		// reading 32 bytes
  outb(iobase + 0x0B, 0);		// count high
  outb(iobase + 0x08, 0);		// start DMA at 0
  outb(iobase + 0x09, 0);		// start DMA high
  outb(iobase, 0x0A);		// start the read
  puts("MAC address: ");
  for(int i = 0; i < 32; i++)
  {
    prom[i] = inb(iobase + 0x10);
    if(i < 6)
    {
      itoa(prom[i], b, 16);
      puts(b);
      if(i != 5) putc(':');
    }
  }
  putc('\n');
  //beállítsuk a MAC-címet a PAR0..PAR5 regisztereken keresztül
  //állítsuk a page-et 1-re (a PAR0..PAR5 regiszterek azon a page-en vannak)
  uint8_t cr = inb(iobase);
  uint8_t ncr = cr;
  ncr |= 1 << 6;
  ncr &= ~(1 << 7);
  outb(iobase, ncr);
  io_wait();
  puts("ne2k_pci: page set to 1\n");
  puts("ne2k_pci: writing mac address");
  for (int i = 0; i < 6; i++)
  {
    putc('.');
    outb(iobase + 0x01 + i, prom[i]);
    io_wait();
  }
  putc('\n');
  //visszaállítjuk a page-et 0-ra
  ncr = inb(iobase);
  ncr &= ~(1 << 6);
  ncr &= ~(1 << 7);
  outb(iobase, ncr);
  io_wait();
  puts("ne2k_pci: page set to 0\n");
  puts("ne2k_pci: initialized\n");
}
