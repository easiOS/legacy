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
  int (*write)(void*, size_t); //write int bytes from pointer
  int (*read)(void*, size_t); //read int bytes to pointer
};

struct ethernet_device* ethernet_allocate();
void ethernet_list();

#endif
