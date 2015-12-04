#ifndef H_MEMMGMT
#define H_MEMMGMT

#include "multiboot2.h"

struct memmgmt {
  
}

void memmgmt_init(struct multiboot_mmap_entry* mmap, int mmap_size);

#endif
