#include <networking.h>
#include <stdlib.h>

netif netifs[NETIF_MAX_IF];

netif* netif_allocate(void)
{
	for(int i = 0; i < NETIF_MAX_IF; i++)
	{
		if(netifs[i].flags & 1) continue;
		netifs[i].flags = 1;
		netifs[i].received = 0;
		netifs[i].sent = 0;
		return &netifs[i];
	}
	return NULL;
}

void netif_free(netif* i)
{
	i->flags = 0;
}

const netif* netif_getbyindex(int index)
{
	if(index < 0 || index > NETIF_MAX_IF - 1)
		return NULL;
	if(netifs[index].flags & 1)
		return NULL;
	return (const netif*)&netifs[index];
}

void netif_list(void)
{
	for(int i = 0; i < NETIF_MAX_IF; i++)
	{
		netif* d = &netifs[i];
		if(!(d->flags & 1))
			continue;
		puts("====\n");
		printf("Interface \"%s\"\n", d->name);
		printf("MAC Address: %x:%x:%x:%x:%x:%x\n", d->hwaddr[0], d->hwaddr[1], d->hwaddr[2], d->hwaddr[3], d->hwaddr[4], d->hwaddr[5]);
		printf("IPv4 Address: %d.%d.%d.%d\n", d->inet_addr.addr[0], d->inet_addr.addr[1], d->inet_addr.addr[2], d->inet_addr.addr[3]);
		printf("     Netmask: %d.%d.%d.%d\n", d->inet_addr.mask[0], d->inet_addr.mask[1], d->inet_addr.mask[2], d->inet_addr.mask[3]);
		printf("     Gateway: %d.%d.%d.%d\n", d->inet_addr.gway[0], d->inet_addr.gway[1], d->inet_addr.gway[2], d->inet_addr.gway[3]);
	}
}