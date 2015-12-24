#include <net/routing.h>
#include <dev/ethernet.h>
#include <string.h>

struct routing_table_entry rtable[32];
uint32_t rtablebits = 0;

void routing_table_add(uint8_t* dest, uint8_t* mask, uint8_t* gate, char* ifac)
{
  int slot = -1;
  for(int i = 0; i < 32; i++)
  {
    if(!(rtablebits >> i & 1))
    {
      slot = i;
      break;
    }
  }
  if(slot == -1) return;
  memcpy(rtable[slot].dest, dest, 4);
  memcpy(rtable[slot].mask, mask, 4);
  memcpy(rtable[slot].gate, gate, 4);
  memcpy(rtable[slot].ifac, ifac, 8);
  rtable[slot].ifac[7] = '\0';
  rtable[slot].metric = 0;
}

void routing_table_remove(int i)
{
  if(i > 33 || i < 0) return;
  rtablebits &= ~(1 << i);
}

int routing_table_getif(uint8_t* dest, uint8_t* src)
{
  int fullzero = 0;
  int ret = -1;
  int max = 0;
  for(int i = 0; i < 32; i++)
  {
    if(rtablebits >> i & 1)
    {
      if(!rtable[i].dest[0] && !rtable[i].dest[1] && !rtable[i].dest[2] && !rtable[i].dest[3])
      {
        fullzero = i;
        break;
      }
      int match = 0;
      for(int j = 0; j < 4; j++)
      {
        if(dest[j] == rtable[i].dest[j]) match++;
      }
      if(match > max) ret = i;
    }
  }
  for(int i = 0; i < 8; i++)
  {
    if(strcmp(ethernet_getif(i)->name, rtable[ret].ifac) == 0)
    {
      ret = i;
      break;
    }
  }
  return (ret == -1) ? fullzero : ret;
}
