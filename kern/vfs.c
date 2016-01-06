#include <vfs.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

fs_node_t* root = NULL;
fs_node_t* initramfs = NULL;

struct initramfs_header* initramfs_header = NULL;

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
  if(vfs->magic != 0x45524653) return;
  initramfs = malloc(sizeof(fs_node_t));
  initramfs->name[0] = 'B'; root->name[1] = ':'; root->name[2] = '\0';
  initramfs->flags = FS_DIRECTORY;
  void* f = vfs->files;
  for(int i = 0; i < vfs->count; i++)
  {
    struct initramfs_file* file = f;
    puts("File:\n");
    printf("  Name: %s*\n", file->name);
    printf("  Size: %d bytes\n", file->size);
    f += sizeof(struct initramfs_file) + file->size;
  }
  initramfs_header = vfs;
}

struct initramfs_header* vfs_open()
{
  return initramfs_header;
}