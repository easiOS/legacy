//Port of the e100 driver from iPXE
//hello darkness my old friend
#include <dev/pci/e100.h>
#include <dev/ethernet.h>
#include <dev/pci.h>
#include <dev/timer.h>
#include <port.h>
#include <stdio.h>
#include <stdlib.h>

void e100_irq(struct ethernet_device* dev, int enable)
{
	outw(dev->iobase + SCBCmd, enable ? (SCBMaskEarlyRx | SCBMaskFlowCtl) : SCBMaskAll);
}

void e100_hw_reset(struct ethernet_device* dev)
{
	outl(dev->iobase + CSRPort, PortPartialReset);
	inw(dev->iobase + SCBStatus);
	sleep(2);

	outl(dev->iobase + CSRPort, PortReset);
	inw(dev->iobase + SCBStatus);
	sleep(2);

}

int e100_scb_cmd_wait(struct ethernet_device* dev)
{
	int rc, wait = 1000;
	for(; wait && (rc = inb(dev->iobase + SCBCmd)); wait--)
		sleep(1);
	if(!wait)
		printf("e100: scb_cmd_wait timeout!\n");
	return rc;
}

int e100_scb_cmd(struct ethernet_device* dev, uint32_t ptr, uint8_t cmd)
{
	int rc;
	rc = e100_scb_cmd_wait(dev);
	if(!rc)
	{
		outl(dev->iobase + SCBPointer, ptr);
		outb(dev->iobase + SCBCmd, cmd);
	}
	return rc;
}

void e100_init_eeprom(struct ethernet_device* dev)
{
	//struct e100_private* p = dev->custom;

}

void e100init(uint8_t bus, uint8_t slot)
{
	printf("e100: init on bus %x slot %x\n", bus, slot);
	uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
  	if(vendor == 0xFFFF)
  	{
  		puts("e100: init failed: device not found on specified bus:slot\n");
  		return;
  	}
  	uint16_t device = pci_config_read_word(bus, slot, 0, 2);
  	if(device != 0x1038)
  	{
    	puts("e100: init failed: device is not compatible\n");
    	return;
  	}
	struct ethernet_device* dev = ethernet_allocate();
	if(!dev)
	{
		puts("e100: cannot allocate ethernet device\n");
		return;
  	}
	dev->iobase = ((pci_config_read_dword(bus, slot, 0, 0x14) >> 16) & 0xFFFFFFFC);
	dev->memory = pci_config_read_dword(bus, slot, 0, 0x10) & 0xFFFFFFF0;
	dev->custom = malloc(sizeof(struct e100_private));
	printf("I/O ports: 0x%x\nMemory at 0x%x\nStarting dumping memory...\n\n", dev->iobase, dev->memory);
	e100_irq(dev, 0);
	e100_scb_cmd(dev, 0, RUAddrLoad);
	
}