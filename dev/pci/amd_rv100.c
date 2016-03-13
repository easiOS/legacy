//Advanced Micro Devices RV100 Mobility Radeon driver
#include <dev/pci/amd_rv100.h>
#include <dev/pci.h>
#include <dev/graphics.h>
#include <stdio.h>
#include <port.h>
#include <video.h>

//REM: https://github.com/torvalds/linux/blob/master/drivers/gpu/drm/radeon/reg_srcs/r100
//     https://github.com/torvalds/linux/blob/master/drivers/gpu/drm/radeon/radeon_device.c
//     https://raw.githubusercontent.com/torvalds/linux/master/drivers/gpu/drm/radeon/radeon_reg.h
//     https://github.com/torvalds/linux/blob/master/drivers/gpu/drm/radeon/radeon_device.c#L1277

#define RADEON_ASIC_RESET_DATA 0x39d5e86b
#define RADEON_DST_HEIGHT                   0x1410
#define RADEON_DST_WIDTH                    0x140c

//256 ports

void amdrv100set_mode(vga_card* v, int64_t w, int64_t h, uint8_t bpp)
{
  puts("amd_rv100: set mode stub\n");
}

void amdrv100init(uint8_t bus, uint8_t slot)
{
  printf("amd_rv100: init on bus:slot %x:%x\n", bus, slot);
  uint16_t vendor = pci_config_read_word(bus, slot, 0, 0);
  if(vendor == 0xFFFF)
  {
    puts("amd_rv100: init failed: device not found on specified bus:slot\n");
    return;
  }
  uint16_t device = pci_config_read_word(bus, slot, 0, 2);
  if(device != 0x4c59)
  {
    puts("amd_rv100: init failed: device is not compatible\n");
    return;
  }
  uint32_t fbaddr = pci_config_read_dword(bus, slot, 0, 0x10) & ~0xF;
  uint32_t port = pci_config_read_dword(bus, slot, 0, 0x14) & ~0x3;
  printf("amd_rv100: framebuffer addr: 0x%x port: 0x%x\n", fbaddr, port);
  vga_card* v = graphics_add_card();
  if(!v) return;
  v->bus = bus;
  v->slot = slot;
  v->data[0] = fbaddr;
  v->data[1] = port;
  pci_config_write_byte(bus, slot, 0, 0x3d, 1);
  pci_config_write_byte(bus, slot, 0, 0x3c, 11);
  vdestroy();
  vinit(1024, 768, 32, 1024 * 4, fbaddr);
  printf("amd_rv100: finished.\n");
}
