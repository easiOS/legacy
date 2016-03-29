#include <networking.h>
#include <port.h>
#include <dtables.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <drivers/timer.h>
#include <drivers/pci.h>
#include <drivers/pci/e1000.h>

uint8_t e1000_mmio_read8(uint32_t addr)
{
	return *((volatile uint8_t*)(addr));
}

uint16_t e1000_mmio_read16(uint32_t addr)
{
	return *((volatile uint16_t*)(addr));
}

uint32_t e1000_mmio_read32(uint32_t addr)
{
	return *((volatile uint32_t*)(addr));
}

uint64_t e1000_mmio_read64(uint32_t addr)
{
	return *((volatile uint64_t*)(addr));
}

void e1000_mmio_write8(uint32_t addr, uint8_t val)
{
	(*((volatile uint8_t*)(addr)))=(val);
}

void e1000_mmio_write16(uint32_t addr, uint16_t val)
{
	(*((volatile uint16_t*)(addr)))=(val);
}

void e1000_mmio_write32(uint32_t addr, uint32_t val)
{
	(*((volatile uint32_t*)(addr)))=(val);
}

void e1000_mmio_write64(uint32_t addr, uint64_t val)
{
	(*((volatile uint64_t*)(addr)))=(val);
}

void e1000_writecmd(netif* dev, uint16_t addr, uint32_t val)
{
	struct e1000_private* p = (struct e1000_private*)dev->private;
	if(p->bar_type == 0)
	{
		e1000_mmio_write32(p->memory + addr, val);
	}
	else
	{
		outl(p->iobase, addr);
		outl(p->iobase + 4, val);
	}
}

uint32_t e1000_readcmd(netif* dev, uint16_t addr)
{
	struct e1000_private* p = (struct e1000_private*)dev->private;
	if(p->bar_type == 0)
		return e1000_mmio_read32(p->memory + addr);
	else
	{
		outl(p->iobase, addr);
		return inl(p->iobase + 4);
	}
}

int e1000_detect_eeprom(netif* dev)
{
	struct e1000_private* p = (struct e1000_private*)dev->private;
	uint32_t val = 0;
	e1000_writecmd(dev, REG_EEPROM, 0x1);

	for(int i = 0; i < 1000 && !p->eeprom_exists; i++)
	{
		val = e1000_readcmd(dev, REG_EEPROM);
		if(val & 0x10)
			p->eeprom_exists = 1;
		else
			p->eeprom_exists = 0;
	}
	return p->eeprom_exists;
}

uint32_t e1000_eeprom_read(netif* dev, uint8_t addr)
{
	struct e1000_private* p = (struct e1000_private*)dev->private;
	uint16_t data = 0;
	uint32_t tmp = 0;
	if(p->eeprom_exists)
	{
		e1000_writecmd(dev, REG_EEPROM, (1) | ((uint32_t)(addr) << 8));
		while(!((tmp = e1000_readcmd(dev, REG_EEPROM)) & (1 << 4)));
	}
	else
	{
		e1000_writecmd(dev, REG_EEPROM, (1) | ((uint32_t)(addr) << 2));
		while(!((tmp = e1000_readcmd(dev, REG_EEPROM)) & (1 << 1)));
	}
	data = (uint16_t)((tmp >> 16) & 0xFFFF);
	return data;
}

int e1000_read_mac(netif* dev)
{
	struct e1000_private* p = (struct e1000_private*)dev->private;
	if(p->eeprom_exists)
	{
		uint32_t tmp;
		tmp = e1000_eeprom_read(dev, 0);
		dev->hwaddr[0] = tmp & 0xff;
		dev->hwaddr[1] = tmp >> 8;
		tmp = e1000_eeprom_read(dev, 1);
		dev->hwaddr[2] = tmp & 0xff;
		dev->hwaddr[3] = tmp >> 8;
		tmp = e1000_eeprom_read(dev, 2);
		dev->hwaddr[4] = tmp & 0xff;
		dev->hwaddr[5] = tmp >> 8;
	}
	else
	{
		uint8_t* membase_mac_8 = (uint8_t*)(p->memory + 0x5400);
		uint32_t* membase_mac_32 = (uint32_t*)(p->memory + 0x5400);
		if(membase_mac_32[0] != 0)
		{
			for(int i = 0; i < 6; i++)
			{
				dev->hwaddr[i] = membase_mac_8[i];
			}
		}
		else
			return 0;
	}
	return 1;
}

void e1000_enable_int(netif* dev)
{
	e1000_writecmd(dev, REG_IMASK, 0x1f6dc);
	e1000_writecmd(dev, REG_IMASK, 0xff & ~4);
	e1000_readcmd(dev, 0xc0);
}

void e1000_rxinit(netif* dev)
{
	struct e1000_private* p = (struct e1000_private*)dev->private;
	uint8_t* ptr;
	struct e1000_rx_desc* descs;
	printf("e1000: setting up rx buffer\n");
	ptr = (uint8_t*)malloc(sizeof(struct e1000_rx_desc)*E1000_NUM_RX_DESC + 16);

	descs = (struct e1000_rx_desc*)ptr;
	for(int i = 0; i < E1000_NUM_RX_DESC; i++)
	{
		p->rx_descs[i] = (struct e1000_rx_desc*)((uint8_t*)descs + i*16);
		p->rx_descs[i]->addr = (uint64_t)(uint32_t)(uint8_t*)malloc(8192 + 16);
		p->rx_descs[i]->status = 0;
	}

	e1000_writecmd(dev, REG_TXDESCLO, (uint32_t)ptr);
	e1000_writecmd(dev, REG_TXDESCHI, 0);

	e1000_writecmd(dev, REG_RXDESCLO, (uint64_t)(uint32_t)ptr);
	e1000_writecmd(dev, REG_RXDESCHI, 0);

	e1000_writecmd(dev, REG_RXDESCLEN, E1000_NUM_RX_DESC * 16);

	e1000_writecmd(dev, REG_RXDESCHEAD, 0);
	e1000_writecmd(dev, REG_RXDESCTAIL, E1000_NUM_RX_DESC-1);
	p->rx_cur = 0;
	e1000_writecmd(dev, REG_RCTRL, RCTL_EN|RCTL_SBP|RCTL_UPE|RCTL_MPE|RCTL_LBM_NONE|RTCL_RDMTS_HALF|RCTL_SECRC|RCTL_BSIZE_2048);
}

void e1000_txinit(netif* dev)
{
	uint8_t* ptr;
	struct e1000_private* p = (struct e1000_private*)dev->private;
	struct e1000_tx_desc* descs;
	printf("e1000: setting up tx buffer\n");
	ptr = (uint8_t*)malloc(sizeof(struct e1000_tx_desc)*E1000_NUM_TX_DESC + 16);

	descs = (struct e1000_tx_desc*)ptr;
	for(int i = 0; i < E1000_NUM_TX_DESC; i++)
	{
		p->tx_descs[i] = (struct e1000_tx_desc*)((uint8_t*)descs + i*16);
		p->tx_descs[i]->addr = 0;
		p->tx_descs[i]->cmd = 0;
		p->tx_descs[i]->status = TSTA_DD;
	}

	e1000_writecmd(dev, REG_TXDESCHI, 0);
	e1000_writecmd(dev, REG_TXDESCLO, (uint32_t)ptr);

	e1000_writecmd(dev, REG_TXDESCLEN, E1000_NUM_TX_DESC * 16);

	e1000_writecmd(dev, REG_TXDESCHEAD, 0);
	e1000_writecmd(dev, REG_TXDESCTAIL, 0);
	p->tx_cur = 0;
	e1000_writecmd(dev, REG_TCTRL, TCTL_EN
		| TCTL_PSP
		| (15 << TCTL_CT_SHIFT)
		| (64 << TCTL_COLD_SHIFT)
		| TCTL_RTLC);

	e1000_writecmd(dev, REG_TCTRL, 0b0110000000000111111000011111010);
	e1000_writecmd(dev, REG_TIPG, 0x60200a);
}

void e1000_write(netif* dev, void* buf, int len)
{
	
	struct e1000_private* p = (struct e1000_private*)dev->private;
	//printf("e1000: sending frame from buf at 0x%x with length %x...", buf, len);

	p->tx_descs[p->tx_cur]->addr = (uint32_t)buf;
	p->tx_descs[p->tx_cur]->length = 14 + len;
	p->tx_descs[p->tx_cur]->cmd = CMD_EOP | CMD_IFCS | CMD_RS | CMD_RPS | 0x02; //0x02 = insert CRC
	p->tx_descs[p->tx_cur]->status = 0;
	uint8_t old_cur = p->tx_cur;
	p->tx_cur = (p->tx_cur + 1) % E1000_NUM_TX_DESC;
	e1000_writecmd(dev, REG_TXDESCTAIL, p->tx_cur);
	int timeout = 100;
	while(!(p->tx_descs[old_cur]->status & 0xff))
	{
		sleep(1);
		timeout--;
		if(!timeout)
		{
			//puts("timed out, packet dropped\n");
		}
	}
}

void e1000_check_link(netif* dev)
{
	unsigned status = e1000_readcmd(dev, REG_STATUS);
	printf("e1000: link is %s\n", (status & 2) ? "up" : "down");
	dev->link = status & 2;
}

void e1000_reset(netif* dev)
{
	/*unsigned pbs, pba, ctrl, status;
	printf("e1000: device reset\n");

	//note: reset delay is 20ms

	ctrl = e1000_readcmd(dev, REG_CTRL);
	e1000_writecmd(dev, REG_CTRL, ctrl | 0x04000000); //send reset command
	sleep(20);

	//default config
//	ctrl |= ()*/
}

netif* dfhdevice;

static void e1000_handler(registers_t regs)
{
	struct e1000_private* p = (struct e1000_private*)dfhdevice->private;
	unsigned status = e1000_readcmd(dfhdevice, 0xc0);
	if(status & 0x04)
	{
		//startlink??
	}
	else if(status & 0x10)
	{
		//good threshold
	}
	else if(status & 0x80)
	{
		unsigned short old_cur;
		//int got_packet = 0;

		while((p->rx_descs[p->rx_cur]->status & 0x1))
		{
			//got_packet = 1;

			//ethernet_recv_packet(dfhdevice, (void*)(unsigned)p->rx_descs[p->rx_cur]->addr, p->rx_descs[p->rx_cur]->length);
			puts("e1000: frame received\n");

			p->rx_descs[p->rx_cur]->status = 0;
			old_cur = p->rx_cur;
			p->rx_cur = (p-> rx_cur + 1) % E1000_NUM_RX_DESC;
			e1000_writecmd(dfhdevice, REG_RXDESCTAIL, old_cur);
		}
	}
}

void e1000init(unsigned char bus, unsigned char slot, unsigned char func)
{
	printf("e1000: init\n");
	netif* dev = netif_allocate();
	if(!dev)
	{
		printf("e1000: Cannot allocate ethernet device\n");
		return;
	}
	dev->private = malloc(sizeof(struct e1000_private));
	struct e1000_private* p = (struct e1000_private*)dev->private;
	printf("e1000: reading addresses\n");
	uint32_t bar0 = PCIC_READ_BAR(bus, slot, func, 0);
	p->bar_type = bar0 & 1;
	printf("bartype: %d\n", bar0 & 1);
	p->memory = bar0 & 0xFFFFFFF0;
	p->iobase = bar0 & 0xFFFFFFFC;
	p->eeprom_exists = 0;
	//enable busmastering
	printf("e1000: enabling busmastering\n");
	uint16_t cmd = PCIC_READ_CMD(bus, slot, func);
	cmd |= (1 << 2);
	PCIC_WRITE_CMD(bus, slot, func, cmd);
	//detect eeprom
	printf("e1000: detecting eeprom...");
	puts(e1000_detect_eeprom(dev) ? "yes\n" : "no\n");
	printf("e1000: fetching MAC address\n");
	if(!e1000_read_mac(dev))
	{
		printf("FAILED\n");
		free(dev->private);
		netif_free(dev);
		return;
	}
	for(int i = 0; i<0x80; i++)
		e1000_writecmd(dev, 0x5200 + i * 4, 0);
	uint8_t intline = PCIC_READ_INTL(bus, slot, func);
	dtables_reg(32 + intline, &e1000_handler);
	printf("e1000: registered interrupt %d\n", 32 + intline);
	dfhdevice = dev; //i don't know what i'm doing
	e1000_enable_int(dev);
	e1000_rxinit(dev);
	e1000_txinit(dev);

	uint32_t tctl;
	tctl = e1000_readcmd(dev, REG_TCTRL);
	tctl &= ~((0xff << 4) | (0x3ff << 12));
	tctl |= (TCTL_EN | TCTL_PSP | (0x0f << 4) | (0x40 << 12));
	e1000_writecmd(dev, REG_TCTRL, tctl);

	uint32_t rctl;
	rctl = e1000_readcmd(dev, REG_RCTRL);
	//rctl &= ~(RCTL_BSIZE_4096);
	rctl |= (RCTL_EN | /*RCTL_UPE | RCTL_MPE |*/ RCTL_BAM | RCTL_BSIZE_8192 | RCTL_SECRC);
	e1000_writecmd(dev, REG_RCTRL, rctl);

	e1000_check_link(dev);

	//set link up
	e1000_writecmd(dev, REG_CTRL, 0x20 | ECTRL_SLU); //set link up, activate auto-speed detection

	e1000_check_link(dev);

	dev->write = &e1000_write;
	strcpy(dev->name, "e1000");
	printf("e1000: ready\n");
	//TEST ---------------- TEST
	dev->inet_addr.addr[0] = 192;
	dev->inet_addr.addr[1] = 168;
	dev->inet_addr.addr[2] = 1;
	dev->inet_addr.addr[3] = 128;

	dev->inet_addr.mask[0] = 255;
	dev->inet_addr.mask[1] = 255;
	dev->inet_addr.mask[2] = 0;
	dev->inet_addr.mask[3] = 0;

	dev->inet_addr.gway[0] = 192;
	dev->inet_addr.gway[1] = 168;
	dev->inet_addr.gway[2] = 0;
	dev->inet_addr.gway[3] = 1;
}