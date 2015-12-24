#include <dev/net_loopback.h>
#include <dev/ethernet.h>
#include <stdio.h>
#include <string.h>

void nlb_init()
{
  struct ethernet_device* dev = ethernet_allocate();
  if(!dev)
  {
    puts("loopback: cannot initialize: no more free ethernet slot\n");
    return;
  }
  strcpy(dev->name, "lo");
  memset(dev->mac, 0, 6);
  memset(dev->ipv4_address, 0, 4);
  dev->ipv4_address[0] = 127; dev->ipv4_address[3] = 1;
  memset(dev->ipv4_netmask, 0, 4);
  dev->ipv4_netmask[0] = 0xFF;
  memset(dev->ipv4_gateway, 0, 4);
  dev->ipv4_gateway[0] = 127; dev->ipv4_gateway[3] = 1;
}
