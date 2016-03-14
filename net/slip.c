#include <net/slip.h>
#include <dev/serial.h>
#include <dev/ethernet.h>
#include <krandom.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

eth_dev_t* slip_dev_init(uint16_t port)
{
	eth_dev_t* dev = ethernet_allocate();
	if(!dev)
	{
		printf("Cannot allocate ethernet device\n");
		return NULL;
	}
	strcmp(dev->name, "sl0");
	memset(dev->ipv4_address, 0, 12); //reset ipv4 addresses
	//Generate MAC address from port address
	dev->mac[0] = 0x08; dev->mac[1] = 0x00; dev->mac[2] = 0x27;
	dev->mac[3] = krandom_get() & 0xff;
	dev->mac[4] = port >> 8; dev->mac[5] = port & 0xff;
	if(!serenabled(port))
	{
		serinitport(port);
	}
	sersetmode(port, 1);
	dev->write = &slip_send_packet;
	return dev;
}

int slip_send_packet(void* p, size_t len, uint8_t* dst, eth_dev_t* dev)
{
	uint16_t port = (dev->mac[4] << 8) | dev->mac[5];
	size_t olen = len;
	/* send an initial END character to flush out any data that may
     * have accumulated in the receiver due to line noise
     */
    serwrite(port, END);
    /* for each byte in the packet, send the appropriate character
     * sequence
     */
    uint8_t* pc = p;
    while(len--)
    {
    	switch(*pc)
    	{
    		case END:
    		{
    			serwrite(port, END);
    			serwrite(port, ESC_END);
    			break;
    		}
    		case ESC:
    		{
    			serwrite(port, ESC);
    			serwrite(port, ESC_ESC);
    			break;
    		}
    		default:
    		{
    			serwrite(port, *pc);
    		}
    	}
    	pc++;
    }
    /* tell the receiver that we're done sending the packet
     */
    serwrite(port, END);
	return olen;
}

int slip_send_packet_nodev(void* p, size_t len, uint16_t port)
{
	eth_dev_t dev;
	dev.mac[4] = port >> 8;
	dev.mac[5] = port & 0xff;
    uint8_t ayy[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	return slip_send_packet(p, len, ayy, &dev);
}