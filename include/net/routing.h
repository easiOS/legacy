#ifndef H_ROUTING
#define H_ROUTING

#include <stdint.h>

struct routing_table_entry {
  uint8_t dest[4];
  uint8_t mask[4];
  uint8_t gate[4];
  char ifac[8];
  uint8_t metric; //unused
};

void routing_table_add(uint8_t* dest, uint8_t* mask, uint8_t* gate, char* ifac);
void routing_table_remove(int i);
int routing_table_getif(uint8_t* dest, uint8_t* src);

#endif
