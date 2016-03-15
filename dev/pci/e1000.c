//Intel Ethernet i217

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <port.h>
#include <dtables.h>
#include <dev/pci.h>
#include <dev/pci/e1000.h>
#include <dev/ethernet.h>
#include <dev/timer.h>

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

void e1000_writecmd(struct ethernet_device* dev, uint16_t addr, uint32_t val)
{
	struct e1000_private* p = (struct e1000_private*)dev->custom;
	if(p->bar_type == 0)
	{
		e1000_mmio_write32(dev->memory + addr, val);
	}
	else
	{
		outl(dev->iobase, addr);
		outl(dev->iobase + 4, val);
	}
}

uint32_t e1000_readcmd(struct ethernet_device* dev, uint16_t addr)
{
	struct e1000_private* p = (struct e1000_private*)dev->custom;
	if(p->bar_type == 0)
		return e1000_mmio_read32(dev->memory + addr);
	else
	{
		outl(dev->iobase, addr);
		return inl(dev->iobase + 4);
	}
}

int e1000_detect_eeprom(struct ethernet_device* dev)
{
	struct e1000_private* p = (struct e1000_private*)dev->custom;
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

uint32_t e1000_eeprom_read(struct ethernet_device* dev, uint8_t addr)
{
	struct e1000_private* p = (struct e1000_private*)dev->custom;
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

int e1000_read_mac(struct ethernet_device* dev)
{
	struct e1000_private* p = (struct e1000_private*)dev->custom;
	if(p->eeprom_exists)
	{
		uint32_t tmp;
		tmp = e1000_eeprom_read(dev, 0);
		dev->mac[0] = tmp & 0xff;
		dev->mac[1] = tmp >> 8;
		tmp = e1000_eeprom_read(dev, 1);
		dev->mac[2] = tmp & 0xff;
		dev->mac[3] = tmp >> 8;
		tmp = e1000_eeprom_read(dev, 2);
		dev->mac[4] = tmp & 0xff;
		dev->mac[5] = tmp >> 8;
	}
	else
	{
		uint8_t* membase_mac_8 = (uint8_t*)(dev->memory + 0x5400);
		uint32_t* membase_mac_32 = (uint32_t*)(dev->memory + 0x5400);
		if(membase_mac_32[0] != 0)
		{
			for(int i = 0; i < 6; i++)
			{
				dev->mac[i] = membase_mac_8[i];
			}
		}
		else
			return 0;
	}
	return 1;
}

void e1000_enable_int(struct ethernet_device* dev)
{
	e1000_writecmd(dev, REG_IMASK, 0x1f6dc);
	e1000_writecmd(dev, REG_IMASK, 0xff & ~4);
	e1000_readcmd(dev, 0xc0);
}

void e1000_rxinit(struct ethernet_device* dev)
{
	struct e1000_private* p = (struct e1000_private*)dev->custom;
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

void e1000_txinit(struct ethernet_device* dev)
{
	uint8_t* ptr;
	struct e1000_private* p = (struct e1000_private*)dev->custom;
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

int e1000_write(void* buf, size_t len, uint8_t* dst, struct ethernet_device* dev)
{
	
	struct e1000_private* p = (struct e1000_private*)dev->custom;
	//printf("e1000: sending frame from buf at 0x%x with length %x...", buf, len);

	p->tx_descs[p->tx_cur]->addr = (uint32_t)buf;
	p->tx_descs[p->tx_cur]->length = sizeof(struct ethernet_frame) + len + 4;
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
			goto timeout;
		}
	}
	//puts("sent!\n");
	goto ok;

	timeout:
	len = 0;
	ok:
	return len;
}

void e1000_check_link(struct ethernet_device* dev)
{
	uint32_t status = e1000_readcmd(dev, REG_STATUS);
	printf("e1000: link is %s\n", (status & 2) ? "up" : "down");
	dev->link = status & 2;
}

void e1000_reset(struct ethernet_device* dev)
{
	/*uint32_t pbs, pba, ctrl, status;
	printf("e1000: device reset\n");

	//note: reset delay is 20ms

	ctrl = e1000_readcmd(dev, REG_CTRL);
	e1000_writecmd(dev, REG_CTRL, ctrl | 0x04000000); //send reset command
	sleep(20);

	//default config
//	ctrl |= ()*/
}

struct ethernet_device* dfhdevice;

static void e1000_handler(registers_t regs)
{
	struct e1000_private* p = (struct e1000_private*)dfhdevice->custom;
	uint32_t status = e1000_readcmd(dfhdevice, 0xc0);
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
		uint16_t old_cur;
		//int got_packet = 0;

		while((p->rx_descs[p->rx_cur]->status & 0x1))
		{
			//got_packet = 1;

			ethernet_recv_packet(dfhdevice, (void*)(uint32_t)p->rx_descs[p->rx_cur]->addr, p->rx_descs[p->rx_cur]->length);

			p->rx_descs[p->rx_cur]->status = 0;
			old_cur = p->rx_cur;
			p->rx_cur = (p-> rx_cur + 1) % E1000_NUM_RX_DESC;
			e1000_writecmd(dfhdevice, REG_RXDESCTAIL, old_cur);
		}
	}
}

void e1000init(uint8_t bus, uint8_t slot)
{
	printf("e1000: init\n");
	struct ethernet_device* dev = ethernet_allocate();
	if(!dev)
	{
		printf("e1000: Cannot allocate ethernet device\n");
		return;
	}
	dev->custom = malloc(sizeof(struct e1000_private));
	struct e1000_private* p = (struct e1000_private*)dev->custom;
	printf("e1000: reading addresses\n");
	uint32_t bar0 = pci_config_read_dword(bus, slot, 0, 0x10);
	p->bar_type = bar0 & 1;
	dev->memory = bar0 & 0xFFFFFFF0;
	dev->iobase = bar0 & 0xFFFFFFFC;
	p->eeprom_exists = 0;
	//enable busmastering
	printf("e1000: enabling busmastering\n");
	uint16_t cmd = pci_config_read_word(bus, slot, 0, 0x04);
	cmd |= (1 << 2);
	pci_config_write_word(bus, slot, 0, 0x04, cmd);
	//detect eeprom
	printf("e1000: detecting eeprom...");
	puts(e1000_detect_eeprom(dev) ? "yes\n" : "no\n");
	printf("e1000: fetching MAC address\n");
	if(!e1000_read_mac(dev))
	{
		printf("FAILED\n");
		free(dev->custom);
		ethernet_free(dev);
		return;
	}
	for(int i = 0; i<0x80; i++)
		e1000_writecmd(dev, 0x5200 + i * 4, 0);
	uint8_t intline = pci_config_read_byte(bus, slot, 0, 0x3c);
	register_interrupt_handler(32 + intline, &e1000_handler);
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
	dev->ipv4_address[0] = 192;
	dev->ipv4_address[1] = 168;
	dev->ipv4_address[2] = 1;
	dev->ipv4_address[3] = 128;

	dev->ipv4_netmask[0] = 255;
	dev->ipv4_netmask[1] = 255;
	dev->ipv4_netmask[2] = 0;
	dev->ipv4_netmask[3] = 0;

	dev->ipv4_gateway[0] = 192;
	dev->ipv4_gateway[1] = 168;
	dev->ipv4_gateway[2] = 0;
	dev->ipv4_gateway[3] = 1;
	//uint32_t zero = 0;
	//uint32_t full = 0xFFFFFFFF;
	//routing_table_add((uint8_t*)&zero, (uint8_t*)&full, dev->ipv4_gateway, "e1000"); //add as default route
}