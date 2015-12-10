#include <dev/pci/ne2k_pci.h>
#include <dev/pci.h>
#include <dev/timer.h>
#include <dev/ethernet.h>
#include <port.h>
#include <dtables.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NE_CMD	 	0x00
#define NE_DATAPORT	0x10	/* NatSemi-defined port window offset. */
#define NE_RESET	0x1f	/* Issue a read to reset, a write to clear. */
#define NE_IO_EXTENT	0x20

#define NESM_START_PG	0x40	/* First page of TX buffer */
#define NESM_STOP_PG	0x80	/* Last page +1 of RX ring */

struct _arp_packet {
  uint16_t htype, ptype;
  uint8_t hlen, plen;
  uint16_t oper;
  uint16_t sha0, sha1, sha2;
  uint16_t spa0, spa1;
  uint16_t tha0, tha1, tha2;
  uint16_t tpa0, tpa1;
};

//i have no idea if this'll work or not tbqh fam
void ne2k_writereg(uint16_t iobase, uint8_t page, uint8_t reg, uint8_t val)
{
  //set page
  uint8_t cr = inb(iobase);
  uint8_t ncr = cr;
  ncr &= 0b00111111;
  ncr |= page << 6;
  outb(iobase, ncr);
  io_wait();
  //write val to reg
  outb(iobase + reg, val);
  io_wait();
  outb(iobase, cr);
}

void ne2k_callback(registers_t regs)
{
  puts("ne2k_pci: irq stub\n");
}

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
  struct ethernet_device* dev = ethernet_allocate();
  if(!dev)
  {
    puts("ne2k_pci: cannot allocate ethernet device\n");
    return;
  }
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
  dev->flags |= 1;
  strcpy(dev->name, "ne2k0");
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
      dev->mac[i] = prom[i];
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
  /*puts("ne2k_pci: setting IRQ to 9...");
  pci_config_write_word(bus, slot, 0, 0x3f, 9);
  register_interrupt_handler(IRQ9, ne2k_callback);
  puts("done!\n");*/
  puts("ne2k_pci: word access\n");
  outb(iobase + 0x0E, 0x49);
  puts("ne2k_pci: activating fullduplex\n");
  outb(iobase, 0xC0 + (1 << 5)); //page 3, nodma
  outb(iobase + 0x01, 0xC0); //enable CONFIG3 writing
  outb(iobase + 0x06, 0x40); //enable fdx
  outb(iobase + 0x01, 0x00);
  outb(iobase, 1 << 5);
  puts("ne2k_pci: sending test packet\n");
  //ne2k_writereg(iobase, 0, 0, 0x22);
  outb(iobase, 0x22);
  puts("ne2k_pci: cmd: start, nodma\n");
  //0xA RBCR0, 0xB RBCR1 packet size
  struct _arp_packet testpacket;
  testpacket.htype = 1; //ethernet
  testpacket.ptype = 0x0800; //ipv4
  testpacket.hlen = 6; //ethernet
  testpacket.plen = 4; //ipv4
  testpacket.oper = 2; //reply
  testpacket.spa0 = 0xC0A8;
  testpacket.spa1 = 0x0201;
  testpacket.tha0  = 0x5254;
  testpacket.tha1 = 0xDEAD;
  testpacket.tha2 = 0xBEEF;
  testpacket.tpa0 = 0xC0A8;
  testpacket.tpa1 = 0x0201;
  puts("ne2k_pci: packet constructed\n");
  outb(iobase + 0xA, sizeof(struct _arp_packet));
  //ne2k_writereg(iobase, 0, 0xA, sizeof(struct _arp_packet));
  puts("ne2k_pci: packet size set\n");
  //ne2k_writereg(iobase, 0, 0x7, inb(iobase + 0x7) | 0b01000000);
  outb(iobase + 0x7, inb(iobase + 0x7) | 0b01000000);
  puts("ne2k_pci: set ISR 6th bit to 1\n");
  //ne2k_writereg(iobase, 0, 0, 0x12);
  outb(iobase, 0x12);
  puts("ne2k_pci: cmd: start, remote write dma\n");
  puts("ne2k_pci: writing packet\n");
  for(int i = 0; i < sizeof(struct _arp_packet); i++)
  {
    putc('.');
    outb(iobase + NE_DATAPORT, ((uint8_t*)&testpacket)[i]);
    io_wait();
  }
  puts("ne2k_pci: waiting for ISR 6th bit to be set\n");
  while(!(inb(iobase + 0x7) >> 6 & 1));
  puts("ne2k_pci: test packet sent\n");
  puts("ne2k_pci: initialized\n");
}
