#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <net/udp.h>
#include <net/ipv4.h>
#include <dev/ethernet.h>

struct {
  udp_recvf f;
  void* p;
} udp_portbinds[65536] = {{NULL, NULL}};

uint16_t udp_checksum(void* addr, void* addr2, void* addr3, size_t count, size_t count2, size_t count3)
{
  register uint32_t sum = 0;
  uint16_t* a = addr;
  uint16_t* a2 = addr2;
  uint16_t* a3 = addr3;
  while(count > 1)
  {
    sum += *a++;
    count -= 2;
  }
  if(count)
  {
    sum += *(uint8_t*)addr;
  }

  while(count2 > 1)
  {
    sum += *a2++;
    count2 -= 2;
  }
  if(count2)
  {
    sum += *(uint8_t*)addr2;
  }

  while(count3 > 1)
  {
    sum += *a3++;
    count3 -= 2;
  }
  if(count3)
  {
    sum += *(uint8_t*)addr3;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return (uint16_t)~sum;
}

udp_header* udp_create(udp_header* header, uint16_t* srca, uint16_t* dsta, uint16_t src, uint16_t dst, void* data, uint16_t len)
{
	if(len > 65536 - 8)
		return 0;
  //pseudoheader
  struct ip4_header ip4h;
  memset(&ip4h, 0, sizeof(struct ip4_header));
  ip4h.version = 4;
  ip4h.ihl = 5;
  ip4h.ttl = 64;
  ip4h.tot_len = htons(sizeof(struct ip4_header) + len);
  ip4h.protocol = IP_PROT_UDP;
  memcpy(&ip4h.saddr, srca, 4);
  memcpy(&ip4h.daddr, dsta, 4);
  ip4h.check = ipv4_checksum(&ip4h, sizeof(struct ip4_header));

  header->src = htons(src);
  header->dst = htons(dst);
  header->length = htons(len + 8);
  header->checksum = 0;
  header->checksum = htons(udp_checksum(&ip4h, header, data, sizeof(ip4h), sizeof(udp_header), len));

  return header;
}

void udp_send(void* srca, void* dsta, uint16_t srcp, uint16_t dstp, void* data, uint16_t len)
{
  udp_header udph;
  ipv4_send_data(dsta, srca, udp_create(&udph, srca, dsta, srcp, dstp, data, len), len + sizeof(udph), IP_PROT_UDP);
}

void udp_receive(void* srca, void* dsta, void* data, size_t len)
{
    udp_header* h = data;
    h->src = ntohs(h->src); h->dst = ntohs(h->dst);
    h->length = ntohs(h->length);
    h->checksum = ntohs(h->checksum);
    if(udp_portbinds[h->dst].f)
    {
      udp_portbinds[h->dst].f(udp_portbinds[h->dst].p, data, len, srca, dsta, h->src, h->dst);
    }
}

int udp_bindport(unsigned short port, udp_recvf f, void* p)
{
  if(udp_portbinds[port].f != NULL)
    return 1;
  udp_portbinds[port].f = f;
  udp_portbinds[port].p = p;
  return 0;
}

void udp_unbindport(unsigned short port)
{
  udp_portbinds[port].f = NULL;
  udp_portbinds[port].p = NULL;
}