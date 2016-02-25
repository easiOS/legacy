#include <net/icmp.h>
#include <stdio.h>
#include <string.h>

void icmp_send_ping_req(uint8_t* dest, uint8_t* src)
{
  printf("Sending ping request to %d.%d.%d.%d\n", dest[0], dest[1], dest[2], dest[3]);
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

void icmp_send_ping_reply(uint8_t* dest, uint8_t* src)
{
  printf("Sending ping reply to %d.%d.%d.%d\n", dest[0], dest[1], dest[2], dest[3]);
  struct {
    struct icmp_header header;
    uint16_t a,b;
    char payload[64];
  } __attribute__((packed)) asd;
  asd.header.type = 0;
  asd.header.code = 0;
  asd.header.checksum = 0;
  strcpy(asd.payload, "easios_ping123");
  ipv4_send_data(dest, src, &asd, sizeof(asd), 1);
}

void icmp_recv_icmp(uint8_t* src, uint8_t* dst, uint8_t* data)
{
  struct icmp_header* header = (struct icmp_header*)data;
  printf("received icmp\n");
  printf("\ttype: %d\n", header->type);
  switch(header->type)
  {
    case 0:
    {
      printf("\tping reply from %d.%d.%d.%d\n", src[0], src[1], src[2], src[3]);
      return;
    }
    case 3:
    {
      printf("\tICMP: destination unreachable: ");
      switch(header->code)
      {
        case 0:
        {
          printf("Destination network unreachable\n");
          break;
        }
        case 1:
        {
          printf("Destination host unreachable\n");
          break;
        }
        case 4:
        {
          printf("Fragmentation required, and DF flag set\n");
          break;
        }
        default:
        {
          printf("Code: %d\n", header->code);
          break;
        }
      }
    }
    case 8:
    {
      icmp_send_ping_reply(src, dst);
      break;
    }
    default:
    {}
  }
}