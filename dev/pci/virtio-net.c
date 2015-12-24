#include <dev/pci/virtio-net.h>
#include <dev/pci.h>
#include <stdint.h>
#include <stdio.h>
#include <dev/ethernet.h>
#include <port.h>
#include <string.h>
#include <stdlib.h>

#define VIRTIO_REG_DFEAT 0x00
#define VIRTIO_REG_GFEAT 0x04
#define VIRTIO_REG_QADDR 0x08
#define VIRTIO_REG_QSIZE 0x0C
#define VIRTIO_REG_QSELE 0x0E
#define VIRTIO_REG_QNOTI 0x10
#define VIRTIO_REG_DSTAT 0x12
#define VIRTIO_REG_ISRST 0x13

#define VIRTIO_STAT_DACK 0x01
#define VIRTIO_STAT_LOAD 0x02
#define VIRTIO_STAT_READ 0x04
#define VIRTIO_STAT_DERR 0x40
#define VIRTIO_STAT_DFLR 0x80

struct virtio_net_data {
  struct virtio_vqueue* vvq;
};

int virtio_net_count = 0;

void virtionetinit(uint8_t bus, uint8_t slot)
{
  printf("virtio-net: on PCI %d:%d\n", bus, slot);
  uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
  if(vendor == 0xFFFF)
  {
    printf("virtio-net: device not found\n");
    return;
  }
  uint16_t device = pci_config_read_word(bus, slot, 0, 2);
  if(vendor != 0x1af4 || device < 0x1000 || device > 0x103f)
  {
    printf("virtio-net: device is not virtio device\n");
    return;
  }
  uint32_t iobase = pci_config_read_dword(bus, slot, 0, 0x10) & ~3;
  uint16_t subsys = pci_config_read_word(bus, slot, 0, 0x2E);
  if(subsys != 01)
  {
    printf("virtio-net: virtio device is not network card\n");
    outb(iobase + VIRTIO_REG_DSTAT, VIRTIO_STAT_DFLR); //send driver failed
    return;
  }
  struct ethernet_device* dev = ethernet_allocate();
  if(!dev)
  {
    puts("virtio-net: cannot initialize: no more free ethernet slot\n");
  }
  dev->iobase = iobase;
  printf("virtio-net: iobase: 0x%x\n", dev->iobase);
  //ack device
  outb(dev->iobase + VIRTIO_REG_DSTAT, VIRTIO_STAT_DACK);
  //driver loaded
  outb(dev->iobase + VIRTIO_REG_DSTAT, VIRTIO_STAT_LOAD);
  //get mac address
  printf("virtio-net: MAC address: ");
  for(int i = 0; i < 6; i++)
  {
    dev->mac[i] = inb(dev->iobase + 0x14 + i);
    printf("%x", dev->mac[i]);
    putc((i != 5) ? ':' : '\n');
  }
  char b[8];
  strcat(dev->name, "vrtnet");
  strcat(dev->name, itoa(virtio_net_count++, b, 16));
  void* qptr = malloc(4096 * 4); //lol
  printf("virtio-net: pointer: 0x%x\n", (uint32_t)qptr);
  while((uint32_t)(qptr) % 4096) qptr++;
  printf("virtio-net: aligned pointer: 0x%x\n", (uint32_t)qptr);
  struct virtio_vqueue* vvq = (struct virtio_vqueue*)qptr;
  outw(dev->iobase + VIRTIO_REG_QADDR, ((uint32_t)vvq) >> 12); //address divided by 4096
  outb(dev->iobase + VIRTIO_REG_DSTAT, VIRTIO_STAT_READ); //driver ready
  dev->custom = malloc(sizeof(struct virtio_net_data));
  ((struct virtio_net_data*)dev->custom)->vvq = vvq;
  
}
