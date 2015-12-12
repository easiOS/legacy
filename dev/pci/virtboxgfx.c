#include <dev/pci/virtboxgfx.h>
#include <port.h>
#include <video.h>

uint8_t vbgfx_bus, vbgfx_slot;

#define VBGFX_IO_INDEX 0
#define VBGFX_IO_DATA 1
#define VBGFX_INDEX_ID               0x0
#define VBGFX_INDEX_XRES             0x1
#define VBGFX_INDEX_YRES             0x2
#define VBGFX_INDEX_BPP              0x3
#define VBGFX_INDEX_ENABLE           0x4
#define VBGFX_INDEX_BANK             0x5
#define VBGFX_INDEX_VIRT_WIDTH       0x6
#define VBGFX_INDEX_VIRT_HEIGHT      0x7
#define VBGFX_INDEX_X_OFFSET         0x8
#define VBGFX_INDEX_Y_OFFSET         0x9
#define VBGFX_INDEX_VIDEO_MEMORY_64K 0xa

uint32_t iobase;

void vbgfxinit(uint8_t bus, uint8_t slot)
{
  char b[64];
  puts("vbgfx: init on PCI bus:slot ");
  puts(itoa(bus, b, 10)); putc(':'); puts(itoa(slot, b, 10));
  putc('\n');
  uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
  if(vendor == 0xFFFF)
  {
    puts("vbgfx: init failed: device not found on specified bus:slot\n");
    return;
  }
  uint16_t device = pci_config_read_word(bus, slot, 0, 2);
  if(vendor != 0x80ee || (device != 0xBEEF && device != 0x7145))
  {
    puts("vbgfx: init failed: device is not compatible\n");
    return;
  }
  vbgfx_bus = bus;
  vbgfx_slot = slot;
  uint32_t bar0, bar1, bar2, bar3, bar4, bar5;
  bar0 = pci_config_read_dword(bus, slot, 0, 0x10);
  puts("vbgfx: bar0: 0x"); puts(itoa(bar0, b, 16)); putc('\n');
  bar1 = pci_config_read_dword(bus, slot, 0, 0x14);
  puts("vbgfx: bar1: 0x"); puts(itoa(bar1, b, 16)); putc('\n');
  bar2 = pci_config_read_dword(bus, slot, 0, 0x18);
  puts("vbgfx: bar2: 0x"); puts(itoa(bar2, b, 16)); putc('\n');
  bar3 = pci_config_read_dword(bus, slot, 0, 0x1C);
  puts("vbgfx: bar3: 0x"); puts(itoa(bar3, b, 16)); putc('\n');
  bar4 = pci_config_read_dword(bus, slot, 0, 0x20);
  puts("vbgfx: bar4: 0x"); puts(itoa(bar4, b, 16)); putc('\n');
  bar5 = pci_config_read_dword(bus, slot, 0, 0x24);
  puts("vbgfx: bar5: 0x"); puts(itoa(bar5, b, 16)); putc('\n');
  uint8_t progif = pci_config_read_word(bus, slot, 0, 0xA) >> 8;
  puts("vbgfx: progif: 0x"); puts(itoa(progif, b, 16)); putc('\n');
  uint8_t status = pci_config_read_word(bus, slot, 0, 0x4);
  uint8_t cmd = pci_config_read_word(bus, slot, 0, 0x6);
  puts("vbgfx: status: 0b"); puts(itoa(status, b, 2)); puts(" cmd: 0b");
  puts(itoa(cmd, b, 2)); putc('\n');
  iobase = 0x01CE;
  uint16_t ver;
  outw(iobase + VBGFX_IO_INDEX, VBGFX_INDEX_ID);
  ver = inw(iobase + VBGFX_IO_DATA);
  if(ver < 0xB0C4 ) //check version
  {
    puts("vbgfx: bad id: "); puts(itoa(ver, b, 16)); putc('\n');
    iobase = 0;
    if(!vgetw()) //if grub didn't give us vbe
      pci_config_write_word(bus, slot, 0, 0x06, 0); //disconnect device
    return;
  }
  vbgfx_set_res(1024, 768);
}

void vbgfx_set_res(uint32_t w, uint32_t h)
{
  if(!iobase) return;
  if(w * h > 1440000) return;
  outw(iobase + VBGFX_IO_INDEX, VBGFX_INDEX_ENABLE);
  outw(iobase + VBGFX_IO_DATA, 0); //disable VBE extensions
  //write resolution
  outw(iobase + VBGFX_IO_INDEX, VBGFX_INDEX_XRES);
  outw(iobase + VBGFX_IO_DATA, w);
  outw(iobase + VBGFX_IO_INDEX, VBGFX_INDEX_YRES);
  outw(iobase + VBGFX_IO_DATA, h);
  //write bpp
  outw(iobase + VBGFX_IO_INDEX, VBGFX_INDEX_BPP);
  outw(iobase + VBGFX_IO_DATA, 32);
  //reset video module
  vdestroy();
  //enable VBE extensions and LFB
  outw(iobase + VBGFX_IO_INDEX, VBGFX_INDEX_ENABLE);
  outw(iobase + VBGFX_IO_DATA, 1 | 0x40);
  //init video module
  int64_t width, height, bpp;
  uint32_t lfb_addr = pci_config_read_dword(vbgfx_bus, vbgfx_slot, 0, 0x10) & ~0xF;
  outw(iobase + VBGFX_IO_INDEX, VBGFX_INDEX_XRES);
  width = inw(iobase + VBGFX_IO_DATA);
  outw(iobase + VBGFX_IO_INDEX, VBGFX_INDEX_YRES);
  height = inw(iobase + VBGFX_IO_DATA);
  outw(iobase + VBGFX_IO_INDEX, VBGFX_INDEX_BPP);
  bpp = inl(iobase + VBGFX_IO_DATA);
  vinit(width, height, bpp, w * 4, lfb_addr);
}

int vbgfx_isinit()
{
  return iobase != 0;
}
