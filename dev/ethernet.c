#include <dev/ethernet.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ETH_N 8

struct ethernet_device ethernet_devices[MAX_ETH_N];

struct ethernet_device* ethernet_allocate()
{
  for(int i = 0; i < MAX_ETH_N; i++)
  {
    if(ethernet_devices[i].flags & 1) continue;
    ethernet_devices[i].flags = 1;
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

int ethernet_send_arp(struct ethernet_device* dev)
{
  struct {
      ethernet_frame_t ethf;
      arp_packet_t arpp;
  } __attribute__((packed)) pckt;
  puts("ethernet: building ARP packet...");
  for(int i = 0; i < 6; i++)
  {
    pckt.ethf.hwaddr_src[i] = dev->mac[i];
    pckt.ethf.hwaddr_dest[i] = 0xFF;
  }
  pckt.ethf.ethertype = 0x0806;
  pckt.arpp.htype = 1;
  pckt.arpp.ptype = 0x0800;
  pckt.arpp.hlen = 6;
  pckt.arpp.plen = 4;
  pckt.arpp.op = 2;
  for(int i = 0; i < 6; i++)
  {
    ((uint8_t*)&pckt.arpp.sha0)[i] = dev->mac[i];
    ((uint8_t*)&pckt.arpp.tha0)[i] = dev->mac[i];
  }
  for(int i = 0; i < 4; i++)
  {
    ((uint8_t*)&pckt.arpp.spa0)[i] = dev->ipv4_address[i];
    ((uint8_t*)&pckt.arpp.tpa0)[i] = dev->ipv4_address[i];
  }
  puts("sending...");
  dev->write(&pckt, sizeof(pckt), dev);
  puts("sent!\n");
  return 0;
}

const struct ethernet_device* ethernet_getif(int id)
{
  return (const struct ethernet_device*)&ethernet_devices[id];
}
