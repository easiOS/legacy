#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

struct initramfs_node {
  char name[128];
  uint16_t flags;
};

struct initramfs_directory {
  char name[128];
  uint16_t flags;
  uint32_t subnode_n;
  struct initramfs_node nodes[0];
};

struct initramfs_file {
  char name[128];
  uint16_t flags;
  uint32_t size;
  uint8_t data[0];
};

struct initramfs_header {
  uint32_t magic; //0xC0C0A123
  struct initramfs_directory root;
};

int main(int argc, char** argv)
{
  DIR* dir;
  struct dirent* entry;
  if(!(dir = opendir("initramfs"))) return 1;
  if(!(entry = readdir(dir))) return 1;
  do
  {
    if(entry->d_type == DT_REG)
    {
      printf("%s\n", entry->d_name);
    }
  } while(entry = readdir(dir));
  closedir(dir);
  return 0;
}
