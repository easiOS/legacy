#ifndef H_MEM
#define H_MEM

#include <multiboot2.h>
#include <stdint.h>

struct memmgmt {
  uint64_t address; //start of free memory
  uint64_t blocks_n; //number of 16kB blocks
  uint8_t blocks[2048]; //
} mmgmt_conf;

void memmgmt_init(struct multiboot_mmap_entry* mmap, int mmap_size);

#endif
