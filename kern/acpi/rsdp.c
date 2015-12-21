#include <acpi.h>
#include <stddef.h>
#include <string.h>

static struct rsdp_desc* rsdp_ptr = NULL;

struct rsdp_desc* acpi_findrsdp()
{
  size_t i = 0;
  char rsdp_sign[] = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' '};
  for(i = 0x000E0000; i < 0x00100000; i += 16)
  {
    if(memcmp((char*)i, rsdp_sign, 8) == 0)
    {
      uint8_t* table_bytes = (uint8_t*)i;
      uint8_t sum = 0;
      for(int j = 0; j < sizeof(struct rsdp_desc); j++)
      {
        sum += table_bytes[j];
      }
      if(sum == 0) break;
    }
  }
  rsdp_ptr = (struct rsdp_desc*)i;
  return (struct rsdp_desc*)i;
}

struct rsdp_desc* acpi_getrsdp()
{
  return rsdp_ptr;
}
