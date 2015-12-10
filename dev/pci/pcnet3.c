#include <dev/pci/pcnet3.h>
#include <stdio.h>
#include <stdlib.h>
#include <dev/pci.h>
#include <port.h>
#include <string.h>
#include <dev/ethernet.h>
#include <dtables.h>

#define REG_APROM0 0x00
#define REG_APROM4 0x04
#define REG_APROM8 0x08
#define REG_APROMC 0x0C
#define REG_RDP    0x10
#define REG_RAP    0x14
#define REG_RESET  0x18
#define REG_BDP    0x1C

#define PCNET32_DWIO_RDP	0x10
#define PCNET32_DWIO_RAP	0x14
#define PCNET32_DWIO_RESET	0x18
#define PCNET32_DWIO_BDP	0x1C

struct pcnet32_rx_head {
	uint32_t	base;
	uint16_t	buf_length;	/* two`s complement of length */
	uint16_t	status;
	uint32_t	msg_length;
	uint32_t	reserved;
};

struct pcnet32_tx_head {
	uint32_t	base;
	uint16_t	length;		/* two`s complement of length */
	uint16_t	status;
	uint32_t	misc;
	uint32_t	reserved;
};


struct pcnet32_init_block {
	uint16_t	mode;
	uint16_t	tlen_rlen;
	uint8_t	phys_addr[6];
	uint16_t	reserved;
	uint32_t	filter[2];
	/* Receive and transmit ring base, along with extra bits. */
	uint32_t	rx_ring;
	uint32_t	tx_ring;
} pcnet32_init_block;

uint32_t rx_buffers, tx_buffers;
uint8_t* rdes, *tdes;
int rx_buffer_ptr = 0, tx_buffer_ptr = 0;

uint16_t iobase;

static uint16_t pcnet32_dwio_read_csr(unsigned long addr, int index)
{
	outl(addr + PCNET32_DWIO_RAP, index);
	return inl(addr + PCNET32_DWIO_RDP) & 0xffff;
}

static void pcnet32_dwio_write_csr(unsigned long addr, int index, uint16_t val)
{
	outl(addr + PCNET32_DWIO_RAP, index);
	outl(addr + PCNET32_DWIO_RDP, val);
}

static uint16_t pcnet32_dwio_read_bcr(unsigned long addr, int index)
{
	outl(addr + PCNET32_DWIO_RAP, index);
	return inl(addr + PCNET32_DWIO_BDP) & 0xffff;
}

static void pcnet32_dwio_write_bcr(unsigned long addr, int index, uint16_t val)
{
	outl(addr + PCNET32_DWIO_RAP, index);
	outl(addr + PCNET32_DWIO_BDP, val);
}

/*static uint16_t pcnet32_dwio_read_rap(unsigned long addr)
{
	return inl(addr + PCNET32_DWIO_RAP) & 0xffff;
}

static void pcnet32_dwio_write_rap(unsigned long addr, uint16_t val)
{
	outl(addr + PCNET32_DWIO_RAP, val);
}

static void pcnet32_dwio_reset(unsigned long addr)
{
	inl(addr + PCNET32_DWIO_RESET);
}

static int pcnet32_dwio_check(unsigned long addr)
{
	outl(addr + PCNET32_DWIO_RAP, 88);
	return (inl(addr + PCNET32_DWIO_RAP) & 0xffff) == 88;
}*/

int pcnet32_driverowns(uint8_t* des, int idx)
{
	return (des[16 * idx + 7] & 0x80) == 0;
}

int pcnet32_nexttxidx(int current)
{
	int ret = current + 1;
	if(ret == 8)
		ret = 0;
	return ret;
}

int pcnet32_nextrxidx(int current)
{
	int ret = current + 1;
	if(ret == 32)
		ret = 0;
	return ret;
}

void pcnet32_initde(uint8_t* des, int idx, int is_tx)
{
	memset(&des[idx * 16], 0, 16);
	uint32_t buf_addr = is_tx ? tx_buffers : rx_buffers;
	*(uint32_t *)&des[idx * 16] = buf_addr + idx * 1548;
	uint16_t bcnt = (uint16_t)(-1548);
	bcnt &= 0x0fff;
  bcnt |= 0xf000;
	*(uint16_t *)&des[idx * 16 + 4] = bcnt;
	if(!is_tx)
		des[idx * 16 + 7] = 0x80;
}

static void pcnet32_callback(registers_t regs)
{
	puts("pcnet3: cb\n");
	pcnet32_dwio_write_csr(iobase, 0, pcnet32_dwio_read_csr(iobase, 0) | 0x7f00);
	pcnet32_dwio_write_csr(iobase, 4, pcnet32_dwio_read_csr(iobase, 4) | 0x26a);
	outb(0x20, 0x20);
}

static int pcnet32_sendpacket(void* packet, size_t len)
{
	if(!pcnet32_driverowns(tdes, tx_buffer_ptr))
	{
		puts("pcnet3: driver doesn't own txbuffer de\n");
		return 0;
	}
	puts("pcnet3: copying data to buffer...");
	//tdes[tx_buffer_ptr * 16] = (uint32_t)packet;
	memcpy((void *)(tx_buffers + tx_buffer_ptr * 16), packet, len);
	tdes[tx_buffer_ptr * 16 + 7] |= 0x2;
	tdes[tx_buffer_ptr * 16 + 7] |= 0x1;
	puts("set de flags...");
	uint16_t bcnt = (uint16_t)(-len);
  bcnt &= 0xfff;
  bcnt |= 0xf000;
	puts("set de...");
  *(uint16_t *)&tdes[tx_buffer_ptr * 16 + 4] = bcnt;
	tdes[tx_buffer_ptr * 16 + 7] |= 0x80;
	while(tdes[tx_buffer_ptr * 16 + 7] | 0x80);
	//pcnet32_dwio_write_csr(iobase, 0, 0x48);
	tx_buffer_ptr = pcnet32_nexttxidx(tx_buffer_ptr);
	puts("done!\n");
	return len;
}

void pcnet3init(uint8_t bus, uint8_t slot)
{
  //uint32_t iobase;
  char b[64];
  puts("Initializing pcnet3 on PCI bus:slot "); itoa(bus, b, 10);
  puts(b); putc(':'); itoa(slot, b, 10); puts(b); putc('\n');
  uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
  if(vendor == 0xFFFF)
  {
    puts("pcnet3: init failed: device not found on specified bus.slot\n");
    return;
  }
  uint16_t device = pci_config_read_word(bus, slot, 0, 2);
  if(vendor != 0x1022 || device != 0x2000)
  {
    puts("pcnet3: init failed: device is not compatible\n");
    return;
  }
  //config
  uint32_t conf = pci_config_read_word(bus, slot, 0, 4) << 16 | pci_config_read_word(bus, slot, 0, 6);
  conf &= 0xFFFF0000;
  conf |= 0x5;
  pci_config_write_word(bus, slot, 0, 4, (uint16_t) (conf >> 16));
  pci_config_write_word(bus, slot, 0, 6, (uint16_t) conf);
  //get iobase
  uint32_t bar0 = (((uint32_t)pci_config_read_word(bus, slot, 0, 0x10)) << 16) | (pci_config_read_word(bus, slot, 0, 0x12) & 0xFFFF);
  itoa(bar0, b, 16);
  puts("pcnet3: BAR0 address: ");
  puts(b);
  putc('\n');
  iobase = ((bar0 >> 16) & 0xFFFFFFFC);
  if(inb(iobase) == 0xFF)
  {
    puts("pcnet3: what teh hell\n");
    return;
  }
  itoa(iobase, b, 16);
  puts("pcnet3: IO base addr: "); puts(b); putc('\n');
  struct ethernet_device* dev = ethernet_allocate();
	dev->flags |= 1;
  dev->iobase = iobase;
  dev->irq = 9;
	pci_config_write_byte(bus, slot, 0, 0x3f, 9);
  puts("pcnet3: switching to 32-bit mode\n");
  //set to 32-bit mode
  inl(iobase + 0x18);
  inw(iobase + 0x14);
	//sleep(10);
	outl(iobase + REG_RDP, 0);
  //get MAC
  puts("pcnet3: MAC address: ");
  for(int i = 0; i < 6; i++)
  {
    int macaddr = inb(iobase + REG_APROM0 + i);
    itoa(macaddr, b, 16);
		pcnet32_init_block.phys_addr[i] = macaddr;
    dev->mac[i] = macaddr;
    puts(b); if(i != 5) putc(':');
  }
  putc('\n');
	puts("pcnet3: Register interrupt handler...");
	register_interrupt_handler(IRQ9, &pcnet32_callback);
	puts("done!\n");
	//swstyle -> 2
	puts("pcnet3: setting swtyle to 2\n");
	uint32_t csr58 = pcnet32_dwio_read_csr(iobase, 58);
	csr58 &= 0xfff0;
	csr58 |= 2;
	pcnet32_dwio_write_csr(iobase, 58, csr58);
	//asel
	puts("pcnet3: setting asel\n");
	uint32_t bcr2 = pcnet32_dwio_read_bcr(iobase, 2);
	bcr2 |= 0x2;
	pcnet32_dwio_write_bcr(iobase, 2, bcr2);
	puts("pcnet3: allocating buffers and descriptor entries\n");
	rx_buffers = (uint32_t)malloc(1548 * 32);
	tx_buffers = (uint32_t)malloc(1548 * 8);
	rdes = (uint8_t*)malloc(32 * 16);
	tdes = (uint8_t*)malloc(8 * 16);
	for(int i = 0; i < 32; i++)
	{
		pcnet32_initde(rdes, i, 0);
		if(i < 8)
			pcnet32_initde(tdes, i, 1);
	}
	puts("pcnet3: writing init block address\n");
	pcnet32_init_block.tlen_rlen = 0x30 << 8 | 0x50;
	pcnet32_init_block.mode = 0;
	pcnet32_init_block.rx_ring = (uint32_t)rdes;
	pcnet32_init_block.tx_ring = (uint32_t)tdes;
	pcnet32_dwio_write_csr(iobase, 1, (uint32_t)&pcnet32_init_block);
	pcnet32_dwio_write_csr(iobase, 2, ((uint32_t)&pcnet32_init_block) >> 16);
	uint32_t csr3 = pcnet32_dwio_read_csr(iobase, 3);
	uint32_t csr4 = pcnet32_dwio_read_csr(iobase, 4);
	//nullázások
	csr3 &= ~(1 << 10 | 1 << 2);
	//szettelések
	csr3 |= 1 << 8;
	csr4 |= 1 << 11 | 1 << 10;
	pcnet32_dwio_write_csr(iobase, 3, csr3);
	pcnet32_dwio_write_csr(iobase, 4, csr4);
	uint32_t csr0 = pcnet32_dwio_read_csr(iobase, 0);
	csr0 |= 1 | 1 << 6;
	pcnet32_dwio_write_csr(iobase, 0, csr0);
	puts("pcnet3: waiting for init.");
	while(!(pcnet32_dwio_read_csr(iobase, 0) & 0x100));
	puts("..done! Starting card...");
	csr0 = pcnet32_dwio_read_csr(iobase, 0);
	csr0 &= ~(1 | 1 << 2);
	csr0 |= 1<<1;
	pcnet32_dwio_write_csr(iobase, 0, csr0);
	puts("done!\n");
	puts("pcnet3: card successfully initialized!!\n");
	dev->write = &pcnet32_sendpacket;
	uint8_t fasz[] = {//0x55, 0x55, 0x55, 0x55,0x55, 0x55, 0x55, 0xD5,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0,0,0,0,0,0,
	0x06, 0x08,
	//arp
	0x01, 0x00,
	0x00, 0x08,
	0x06, 0x04,
	0x02, 0x00,
	0, 0,
	0, 0,
	0, 0,
	192, 168,
	2, 1,
	0x08, 0x00,
	0x27, 0x10,
	0xFA, 0x52,
	//0xBF, 0x4D, 0x45, 0xBE
	};
	dev->write(&fasz, sizeof(fasz) / sizeof(uint8_t));
	puts("pcnet3: tested\n");
	//asm("int $0x71");
}
