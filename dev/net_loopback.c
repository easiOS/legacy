#include <dev/net_loopback.h>
#include <dev/ethernet.h>
#include <kernel.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <net/routing.h>

#define NLB_RX_STACK_MAX 32

struct nlb_rx_stack
{
  int32_t p;
  uint8_t* data;
};

int nlb_write(void* data, size_t len, eth_dev_t* dev);
int nlb_read(void* buffer, eth_dev_t* dev);
int nlb_available(eth_dev_t* dev);
int nlb_full(eth_dev_t* dev);

void nlb_init()
{
  struct ethernet_device* dev = ethernet_allocate();
  if(!dev)
  {
    puts("loopback: cannot initialize: no more free ethernet slot. This shouldn't have happened.\n");
    registers_t regs;
    kpanic("LOOPBACK_CANNOT_INIT", regs);
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
  dev->custom = malloc(sizeof(struct nlb_rx_stack)); //rx stack
  ((struct nlb_rx_stack*)dev->custom)->p = -1;
  ((struct nlb_rx_stack*)dev->custom)->data = (uint8_t*)malloc(NLB_RX_STACK_MAX * 1548);
  dev->read = &nlb_read;
  dev->write = &nlb_write;
  dev->available = &nlb_available;
  dev->full = &nlb_full;
  uint8_t nullip[4] = {0, 0, 0, 0};
  uint8_t eightmask[4] = {255, 0, 0, 0};
  routing_table_add((uint8_t*)&nullip, (uint8_t*)&nullip, (uint8_t*)dev->ipv4_address, "lo");
  routing_table_add((uint8_t*)dev->ipv4_address, (uint8_t*)&eightmask, (uint8_t*)dev->ipv4_address, "lo");
}

int nlb_write(void* data, size_t len, eth_dev_t* dev)
{
  if(!dev) return 0;
  struct nlb_rx_stack* stack = (struct nlb_rx_stack*)dev->custom;
  if(!data) return 0;
  if(!len) return 0;
  if(stack->p >= 31) return 0;
  int rlen = len < 1548 ? len : 1548;
  memcpy(&stack->data[stack->p++], data, rlen);
  return rlen;
}

int nlb_read(void* buffer, eth_dev_t* dev)
{
  if(!dev) return 0;
  struct nlb_rx_stack* stack = (struct nlb_rx_stack*)dev->custom;
  if(!buffer) return 0;
  if(stack->p < 0) return 0;
  memcpy(buffer, &stack->data[stack->p--], 1548);
  return 1548;
}

int nlb_available(eth_dev_t* dev)
{
  if(!dev) return 0;
  struct nlb_rx_stack* stack = (struct nlb_rx_stack*)dev->custom;
  return stack->p >= 0;
}

int nlb_full(eth_dev_t* dev)
{
  if(!dev) return 0;
  struct nlb_rx_stack* stack = (struct nlb_rx_stack*)dev->custom;
  return stack->p >= 31;
}
