#ifndef H_MEMORY
#define H_MEMORY

#include <multiboot2.h>
#include <stddef.h>

void kminit(struct multiboot_mmap_entry* mmap, int mmap_size);
void* kmalloc(size_t size);
void kmfree(void* ptr);

#endif
