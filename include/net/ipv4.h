#ifndef H_IPV4
#define H_IPV4

#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

struct ip4_header
{
  uint8_t ihl:4, version:4;
  uint8_t tos;
  uint16_t tot_len;
  uint16_t id;
  uint16_t frag_off;
  uint8_t ttl;
  uint8_t protocol;
  int16_t check;
  uint32_t saddr;
  uint32_t daddr;
} __attribute__((packed));

void ipv4_send_data(uint8_t* dest, uint8_t* src, void* data, size_t len, uint8_t protocol);
void ipv4_recv_data(void* data, size_t len);
uint16_t ipv4_checksum(void* addr, size_t count);

#define IP_PROT_ICMP 0x01
#define IP_PROT_UDP  0x11

#endif
