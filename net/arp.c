#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dev/ethernet.h>
#include <net/arp.h>

#include <dev/timer.h>

struct arp_entry arp_cache[ARP_CACHE_SIZ];

void arp_send_reply(struct ethernet_device* dev, uint16_t* sender_h, uint16_t* sender_p, uint16_t* target_h, uint16_t* target_p)
{
	struct arp_packet p;
	p.htype = ARP_HW_ETHERNET;
	p.ptype = ARP_PROT_IPV4;
	p.hwalen = 6;
	p.protalen = 4;
	p.op = ARP_OP_REP;
	for(int i = 0; i < 3; i++)
	{
		p.sha[i] = (sender_h[i]);
		p.tha[i] = target_h[i];
		target_h[i] = (target_h[i]);
	}
	for(int i = 0; i < 2; i++)
	{
		p.spa[i] = (sender_p[i]);
		p.tpa[i] = target_p[i];
	}
	ethernet_send_packet(dev, &p, sizeof(p), (uint8_t*)target_h, PROT_ARP);
}

void arp_receive(struct ethernet_device* dev, void* data)
{
	struct arp_packet* p = data;
	if(p->ptype != ARP_PROT_IPV4 && p->htype != ARP_HW_ETHERNET
		&& p->hwalen != 6 && p->protalen != 4)
	{
		return;
	}
	switch(p->op)
	{
		case ARP_OP_REQ:
		{
			uint16_t* myip = (uint16_t*)dev->ipv4_address;
			if(/*ntohs*/(myip[0]) == p->tpa[0]
				&& /*ntohs*/(myip[1] == p->tpa[1]))
			{
				arp_send_reply(dev, (uint16_t*)dev->mac, (uint16_t*)dev->ipv4_address, p->sha, p->spa);
			}
			else
			{
				for(int i = 0; i < ARP_CACHE_SIZ; i++)
				{
					if(arp_cache[i].flags & 1)
					{
						if(memcmp(arp_cache[i].protocol, p->tpa, 4) == 0)
						{
							arp_send_reply(dev, (uint16_t*)arp_cache[i].hwaddr, (uint16_t*)arp_cache[i].protocol, p->sha, p->spa);
							break;
						}
					}
				}
			}
		}
		case ARP_OP_REP:
		{
			for(int i = 0; i < 3; i++)
				p->sha[i] = /*ntohs*/(p->sha[i]);
			for(int i = 0; i < 2; i++)
				p->spa[i] = /*ntohs*/(p->spa[i]);
			for(int i = 0; i < ARP_CACHE_SIZ; i++)
			{
				if(!(arp_cache[i].flags & 1))
				{
					memcpy(arp_cache[i].protocol, p->spa, 4);
					memcpy(arp_cache[i].hwaddr, p->sha, 6);
					arp_cache[i].htype = ARP_HW_ETHERNET;
					arp_cache[i].flags |= 1;
					arp_cache[i].expires = ticks() + ARP_CACHE_EXPT;
				}
			}
		}
	}
}