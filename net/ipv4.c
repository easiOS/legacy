#include <net/ipv4.h>
#include <dev/ethernet.h>
#include <stdio.h>
#include <string.h>
#include <net/icmp.h>
#include <net/routing.h>

void ipv4_send_data(uint8_t* dest, uint8_t* src, void* data, size_t len, uint8_t protocol)
{
  printf("ipv4_send_data:\n\tdest: %d.%d.%d.%d\n\tsrc: %d.%d.%d.%d\n",
  dest[0], dest[1], dest[2], dest[3], src[0], src[1], src[2], src[3]);
  void* ip4data = malloc(sizeof(struct ip4_header) + len);
  memcpy(ip4data + sizeof(struct ip4_header), data, len);
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
  if(((uint8_t*)ip4h->daddr)[0] == 127) //loop back
  {
    ipv4_recv_data(ip4data, sizeof(struct ip4_header) + len);
    goto freemem;
  }

  const struct ethernet_device* dev = ethernet_getif(routing_table_getif(dest, src));
  dev->write(ip4data, len + sizeof(struct ip4_header), (struct ethernet_device*)dev);

  freemem:
  free(ip4data);
}

void ipv4_recv_data(void* data, size_t len)
{
  printf("ipv4_recv_data:\n");
  struct ip4_header* ip4h = (struct ip4_header*)data;
  if(ip4h->version != 4)
  {
    printf("\tbad ip version\n");
    return;
  }
  int checksum = ip4h->check;
  ip4h->check = 0;
  int32_t sum = 0;
  for(int i = 0; i < 5; i++)
  {
    sum += ((uint32_t*)ip4h)[i];
  }
  int cchecksum = ~((uint16_t)(sum >> 16) + (uint16_t)(sum));
  if(checksum != cchecksum)
  {
    printf("\tbad checksum got: 0x%x calculated: 0x%x\n", checksum, cchecksum);
    return;
  }
  printf("\tprotocol: %d\n", ip4h->protocol);
  switch(ip4h->protocol)
  {
    case 1: //ICMP
    {
      icmp_recv_icmp((uint8_t*)ip4h->saddr, (uint8_t*)ip4h->daddr, (uint8_t*)(data + sizeof(struct ip4_header)));
      break;
    }
    default:
    {
      printf("\t\tunknown protocol\n");
    }
  }
}
