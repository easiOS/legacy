#ifndef H_DEV_ETHERNET
#define H_DEV_ETHERNET

#include <stdint.h>
#include <stddef.h>

struct ethernet_device {
  uint32_t flags;
  char name[8];
  uint8_t ipv4_address[4];
  uint8_t ipv4_netmask[4];
  uint8_t ipv4_gateway[4];
  uint8_t mac[6];
  uint16_t iobase;
  uint8_t irq;
  void* custom;
  int (*write)(void*, size_t, void*); //write int bytes from pointer
  int (*read)(void*, size_t, void*); //read int bytes to pointer
  int (*available)(void*); //is there anything in the recv buffers
  int (*full)(void*); //are transmit buffers full
};

typedef struct ethernet_frame {
  uint8_t hwaddr_dest[6];
  uint8_t hwaddr_src[6];
  uint16_t ethertype;
} __attribute__((packed)) ethernet_frame_t;

typedef struct arp_packet {
  uint16_t htype, ptype;
  uint8_t hlen, plen;
  uint16_t op;
  uint16_t sha0, sha1, sha2;
  uint16_t spa0, spa1;
  uint16_t tha0, tha1, tha2;
  uint16_t tpa0, tpa1;
} __attribute__((packed)) arp_packet_t;

struct ethernet_device* ethernet_allocate();
void ethernet_list();
int ethernet_send_arp(struct ethernet_device* dev);
const struct ethernet_device* ethernet_getif(int id);

#endif
