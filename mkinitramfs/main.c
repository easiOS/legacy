#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
//these are invalid
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08

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
} __attribute__((packed)) header;

int main(int argc, char** argv)
{
  if(argc > 1)
  {
    printf("Opening ramfs: ");
    FILE* ramfs = fopen("initramfs.bin", "rb");
    if(!ramfs) return 1;
    fread(&header, sizeof(struct initramfs_header), 1, ramfs);
    if(header.magic != 0xC0C0A123) return 2;
    printf("valid!\n");
    struct initramfs_file f;
    for(int i = 0; i < header.root.subnode_n; i++)
    {
      if(feof(ramfs))
      {
        printf("RAMFS EOF WTF\n");
        break;
      }
      memset(&f, 0, sizeof(struct initramfs_file));
      printf("File:\n");
      fread(&f, sizeof(struct initramfs_file), 1, ramfs);
      printf("\tName: %s\n", f.name);
      printf("\tSize: %u\n", f.size);
      printf("NYILACSKA: %u\n", ftell(ramfs));
      fseek(ramfs, ftell(ramfs) + f.size, SEEK_SET);
    }
    fclose(ramfs);
    return 0;
  }
  FILE* outfile = fopen("initramfs.bin", "wb");
  if(!outfile) return 1;
  DIR* dir;
  struct dirent* entry;
  uint32_t i = 0;
  uint32_t filecount = 0;
  if(!(dir = opendir("initramfs"))) return 1;
  if(!(entry = readdir(dir))) return 1;
  do
  {
    if(entry->d_type == DT_REG)
      filecount++;
  } while(entry = readdir(dir));
  closedir(dir);
  printf("Filecount: %d\n", filecount);
  header.root.subnode_n = filecount;
  header.magic = 0xC0C0A123;
  fwrite(&header, sizeof(struct initramfs_header), 1, outfile);
  //second round
  dir = opendir("initramfs");
  entry = readdir(dir);
  do
  {
    if(entry->d_type == DT_REG)
    {
      struct initramfs_file file;
      strcpy(file.name, entry->d_name);
      char fn[256];
      memset(fn, 0, 256);
      strcat(fn, "initramfs/");
      strcat(fn, file.name);
      printf("Processing %s\n", fn);
      FILE* entry_f = fopen(fn, "rb");
      if(!entry_f) break;
      printf("Reading size..");
      fseek(entry_f, 0, SEEK_END);
      file.size = ftell(entry_f);
      fseek(entry_f, 0, SEEK_SET);
      printf(".\n");
      fwrite(&file, sizeof(struct initramfs_file), 1, outfile);
      printf("Copying file...\n"); fflush(stdout);
      int read = 0;
      char buffer[128];
      char ch;
      while((ch = fgetc(entry_f)) != EOF)
        fputc(ch, outfile);
      printf("\nDone!\n");
      fflush(entry_f);
      fclose(entry_f);
      fflush(outfile);
    }
  } while(entry = readdir(dir));
  fclose(outfile);
  printf("Exit!\n");
  return 0;
}
