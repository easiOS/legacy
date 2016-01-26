#include <vfs.h>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <kernel.h>
#include <dev/disk.h>

extern struct lua_apps lua_apps[16];

fs_node_t* root = NULL;
fs_node_t* initramfs = NULL;

struct initramfs_header* initramfs_header = NULL;

extern struct eos_drives drives[4];

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
  int di = -1;
  for(int j = 0;j < 4; j++)
  {
    if(drives[j].letter == 0)
    {
      di = j;
      break;
    }
  }
  if(di == -1)
  {
    printf("  Cannot add more drives\n");
    return;
  }
  drives[di].letter = 'a' + di;
  drives[di].type = 1;
  drives[di].address.virt = (uint32_t)vfs;
  void* f = vfs->files;
  for(int i = 0; i < vfs->count; i++)
  {
    struct initramfs_file* file = f;
    puts("File:\n");
    printf("  Name: %s*\n", file->name);
    printf("  Size: %d bytes\n", file->size);
    void* data = f + sizeof(struct initramfs_file);
    f += sizeof(struct initramfs_file) + file->size;
    if(memcmp(data, "\033Lua", 4) == 0)
    {
      printf("Lua executable detected\n");
      for(int j = 0; j < 16; j++)
      {
        if(lua_apps[j].address != NULL) continue;
        lua_apps[j].address = data;
        strcpy(lua_apps[j].name, file->name);
        printf("    new lua app (ID#%d): %s at address 0x%x\n", j, lua_apps[j].name, lua_apps[j].address);
        break;
      }
    }
  }
  initramfs_header = vfs;
}

struct initramfs_header* vfs_open()
{
  return initramfs_header;
}