#include <net/icmp.h>
#include <stdio.h>
#include <string.h>

void icmp_send_ping_req(uint8_t* dest, uint8_t* src)
{
  printf("Sending ping to %d.%d.%d.%d\n", dest[0], dest[1], dest[2], dest[3]);
  struct {
    struct icmp_header header;
    uint16_t a,b;
    char payload[64];
  } __attribute__((packed)) asd;
  asd.header.type = 8;
  asd.header.code = 0;
  asd.header.checksum = 0;
  strcpy(asd.payload, "easios_ping123");
  ipv4_send_data(dest, src, &asd, sizeof(asd), 1);
}
