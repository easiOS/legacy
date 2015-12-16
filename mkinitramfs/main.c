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

int main(int argc, char** argv)
{
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
  header.count = filecount;
  header.magic = 0xC0C0A123;
  fwrite(&header, sizeof(struct initramfs_header), 1, outfile);
  //second round
  dir = opendir("initramfs");
  entry = readdir(dir);
  uint32_t i = 0;
  do
  {
    if(entry->d_type == DT_REG)
    {
      struct initramfs_file* hf = &(header.files[i]);
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
