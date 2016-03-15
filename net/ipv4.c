#include <net/ipv4.h>
#include <dev/ethernet.h>
#include <stdio.h>
#include <string.h>
#include <net/icmp.h>
#include <net/routing.h>
#include <net/udp.h>

uint16_t ipv4_checksum(void* addr, size_t count)
{
  register uint32_t sum = 0;
  uint16_t* a = addr;
  while(count > 1)
  {
    sum += *a++;
    count -= 2;
  }
  if(count)
  {
    sum += *(uint8_t*)addr;
  }
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return (uint16_t)~sum;
}

void ipv4_send_data(uint8_t* dest, uint8_t* src, void* data, size_t len, uint8_t protocol)
{
  void* ip4data = malloc(sizeof(struct ip4_header) + len);
  memset(ip4data, 0, sizeof(struct ip4_header) + len);
  memcpy(ip4data + sizeof(struct ip4_header), data, len);
  struct ip4_header* ip4h = (struct ip4_header*)ip4data;
  ip4h->version = 4;
  ip4h->ihl = 5;
  ip4h->tos = 0;
  ip4h->id = 0;
  ip4h->frag_off = 0;
  ip4h->ttl = 64;
  ip4h->tot_len = htons(sizeof(struct ip4_header) + len);
  ip4h->protocol = protocol;
  memcpy(&ip4h->saddr, src, 4);
  memcpy(&ip4h->daddr, dest, 4);
  ip4h->check = 0;
  ip4h->check = ipv4_checksum(ip4h, sizeof(struct ip4_header));

  if(((uint8_t*)ip4h->daddr)[0] == 127) //loop back
  {
    ipv4_recv_data(ip4data, sizeof(struct ip4_header) + len);
    goto freemem;
  }

  struct ethernet_device* dev = (struct ethernet_device*)ethernet_getif(routing_table_getif(dest, src));
  uint8_t ayy[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}; //TODO: acquire real MAC through ARP
  ethernet_send_packet(dev, ip4h, sizeof(struct ip4_header) + len, ayy, PROT_IPV4);

  freemem:
  free(ip4data);
}

void ipv4_recv_data(void* data, size_t len)
{
  struct ip4_header* ip4h = (struct ip4_header*)data;
  if(ip4h->version != 4)
  {
    printf("ipv4: bad ip version 0x%x\n", ip4h->version);
    return;
  }
  uint16_t checksum = ip4h->check;
  ip4h->check = ipv4_checksum(ip4h, sizeof(struct ip4_header));
  if(checksum == ip4h->check)
  {
    return;
  }
  switch(ip4h->protocol)
  {
    case 1: //ICMP
    {
      icmp_recv_icmp((uint8_t*)(&ip4h->saddr), (uint8_t*)(&ip4h->daddr), (uint8_t*)(data + sizeof(struct ip4_header)));
      break;
    }
    case 17: //UDP
    {
      printf("ipv4: stub udp recv\n");
      break;
    }
    default:
    {
    }
  }
}
