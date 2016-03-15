#ifndef H_ICMP
#define H_ICMP

#include <stdint.h>
#include <net/ipv4.h>

struct icmp_header {
  uint8_t type;
  uint8_t code;
  uint16_t checksum;
} __attribute__((packed));

struct icmp_ping_header {
    struct icmp_header header;
    uint16_t id, seq;
} __attribute__((packed));

void icmp_send_ping_req(uint8_t* dest, uint8_t* src);
void icmp_recv_icmp(uint8_t* src, uint8_t* dst, uint8_t* data);

#endif
