#include <net/icmp.h>
#include <stdio.h>
#include <string.h>

uint16_t icmp_checksum(void* addr, size_t count)
{
  register uint32_t sum = 0;
  uint16_t* a = addr;
  while(count > 1)
  {
    sum += *a++;
    count -= 2;
  }
  if(count)
  {
    sum += *(uint8_t*)addr;
  }
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return (uint16_t)~sum;
}

void icmp_send_ping_req(uint8_t* dest, uint8_t* src)
{
  printf("Sending ping request to %d.%d.%d.%d\n", dest[0], dest[1], dest[2], dest[3]);
  struct icmp_ping_header asd;
  asd.header.type = 8;
  asd.header.code = 0;
  asd.id = 0xea51;
  asd.seq = 0;
  asd.header.checksum = 0;
  asd.header.checksum = icmp_checksum(&asd, sizeof(asd));
  printf("icmp: pass to ip layer\n");
  ipv4_send_data(dest, src, &asd, sizeof(asd), 1);
}

void icmp_send_ping_reply(uint8_t* dest, uint8_t* src, uint16_t id, uint16_t seq)
{
  printf("Sending ping reply to %d.%d.%d.%d\n", dest[0], dest[1], dest[2], dest[3]);
  struct icmp_ping_header asd;
  memset(&asd, 0, sizeof(asd));
  asd.header.type = 0;
  asd.header.code = 0;
  asd.header.checksum = 0;
  asd.id = id;
  asd.seq = seq;
  asd.header.checksum = 0;

  asd.header.checksum = icmp_checksum(&asd, sizeof(asd));

  ipv4_send_data(dest, src, &asd, sizeof(asd), 1);
}

void icmp_recv_icmp(uint8_t* src, uint8_t* dst, uint8_t* data)
{
  struct icmp_header* header = (struct icmp_header*)data;
  printf("received icmp from %d.%d.%d.%d\n", src[0], src[1], src[2], src[3]);
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
      struct icmp_ping_header* ping = (struct icmp_ping_header*)data;
      uint16_t id = ping->id;
      uint16_t seq = ping->seq;
      icmp_send_ping_reply(src, dst, id, seq);
      break;
    }
    default:
    {}
  }
}