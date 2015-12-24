#include <net/ipv4.h>
#include <dev/ethernet.h>
#include <stdio.h>
#include <string.h>

void ipv4_send_data(uint8_t* dest, uint8_t* src, void* data, size_t len, uint8_t protocol)
{
  printf("ipv4_send_data: data passed\n\tdest: %d.%d.%d.%d\nsrc: %d.%d.%d.%d\n",
  dest[0], dest[1], dest[2], dest[3], src[0], src[1], src[2], src[3]);
  void* ip4data = malloc(sizeof(struct ip4_header) + len);
  struct ip4_header* ip4h = (struct ip4_header*)ip4data;
  ip4h->version = 4;
  ip4h->ihl = 5;
  ip4h->tos = 0;
  ip4h->id = 0;
  ip4h->frag_off = 0;
  ip4h->ttl = 127;
  ip4h->protocol = protocol;
  memcpy((uint8_t*)ip4h->saddr, src, 4);
  memcpy((uint8_t*)ip4h->daddr, dest, 4);
  int32_t sum = 0;
  for(int i = 0; i < 5; i++)
  {
    sum += ((uint32_t*)ip4h)[i];
  }
  ip4h->check = ~((uint16_t)(sum >> 16) + (uint16_t)(sum));
  
}

void ipv4_recv_data(void* data, size_t len)
{

}
