#include <vfs.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct initramfs_file {
  char name[128];
  uint16_t flags;
  uint32_t size;
} __attribute__((packed));

struct initramfs_header {
  uint32_t magic; //0xC0C0A123
  uint32_t count;
  struct initramfs_file files[32];
} __attribute__((packed));

fs_node_t* root = NULL;

uint32_t read_fs(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer)
{
  if(!node) return 0;
  if(node->read) return node->read(node, offset, size, buffer);
  return 0;
}

void vfs_process(void* ptr)
{
  if(!ptr) return;
  struct initramfs_header* vfs = (struct initramfs_header*)ptr;
  if(vfs->magic != 0xC0C0A123) return;
  for(int i = 0; i < 32; i++)
  {
    struct initramfs_file* f = &(vfs->files[i]);
    char buffer[32];
    puts("File:\n");
    puts("  Name: "); puts(f->name); puts("*\n");
    puts("  Size: "); puts(itoa(f->size, buffer, 10)); puts(" bytes\n");
    /*puts("Contents:\n");
    for(int j = 0; j < fp->size; j++)
    {
      putc((char)fp->data[j]);
    }
    putc('\n');*/
  }
}
