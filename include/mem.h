#ifndef H_MEM
#define H_MEM

#include <multiboot2.h>
#include <stdint.h>
#include <stddef.h>

/*struct memmgmt {
  uint64_t address; //start of free memory
  uint64_t blocks_n; //number of 16kB blocks
  uint64_t blocks[2048]; //bit 0: is allocated
                         //bit 1-63: hány blokk tartozik még ehhez
} mmgmt_conf;*/

void memmgmt_init(struct multiboot_mmap_entry* mmap, int mmap_size);
void* mmgmt_alloc(size_t size);
void mmgmt_free(void* ptr);

#endif
