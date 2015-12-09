#include <dev/ethernet.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ETH_N 2

struct ethernet_device ethernet_devices[MAX_ETH_N];

struct ethernet_device* ethernet_allocate()
{
  for(int i = 0; i < MAX_ETH_N; i++)
  {
    if(ethernet_devices[i].flags & 1) continue;
    return &ethernet_devices[i];
  }
  return NULL;
}

void ethernet_list()
{
  for(int i = 0; i < MAX_ETH_N; i++)
  {
    if(!(ethernet_devices[i].flags & 1)) continue;
    char b[32];
    puts("Ethernet device ");
    puts(ethernet_devices[i].name);
    puts("\n  MAC Address: ");
    for(int j = 0; j < 6; j++)
    {
      itoa(ethernet_devices[i].mac[j], b, 16);
      puts(b);
      if(j != 5) putc(':');
    }
    puts("\n  IPv4 Address: ");
    for(int j = 0; j < 4; j++)
    {
      itoa(ethernet_devices[i].ipv4_address[j], b, 10);
      puts(b);
      if(j!=3) putc('.');
    }
    puts("\n  IPv4 Netmask: ");
    for(int j = 0; j < 4; j++)
    {
      itoa(ethernet_devices[i].ipv4_netmask[j], b, 10);
      puts(b);
      if(j!=3) putc('.');
    }
    puts("\n  IPv4 Gateway: ");
    for(int j = 0; j < 4; j++)
    {
      itoa(ethernet_devices[i].ipv4_gateway[j], b, 10);
      puts(b);
      if(j!=3) putc('.');
    }
    puts("\n----------------\n");
  }
}
