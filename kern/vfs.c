#include <vfs.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct initramfs_node {
  char name[128];
  uint16_t flags; // 0x01 = FILE, 0x02 = DIR
};

struct initramfs_directory {
  char name[128];
  uint16_t flags;
  uint32_t subnode_n;
  struct initramfs_node nodes[0];
} __attribute__((packed));

struct initramfs_file {
  char name[128];
  uint16_t flags;
  uint32_t size;
  uint8_t data[0];
} __attribute__((packed));

struct initramfs_header {
  uint32_t magic; //0xC0C0A123
  struct initramfs_directory root;
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
  struct initramfs_file* fp = (struct initramfs_file*)&(vfs->root.nodes[0]);
  if(vfs->magic != 0xC0C0A123) return;
  for(int i = 0; i < vfs->root.subnode_n; i++)
  {
    char buffer[32];
    puts("File:\n");
    puts("  Name: "); puts(fp->name); puts("*\n");
    puts("  Size: "); puts(itoa(fp->size, buffer, 10)); puts(" bytes\n");
    /*puts("Contents:\n");
    for(int j = 0; j < fp->size; j++)
    {
      putc((char)fp->data[j]);
    }
    putc('\n');*/
    fp = (struct initramfs_file*)&(fp->data[fp->size]);
  }
}
