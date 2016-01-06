#ifndef H_VFS
#define H_VFS

#include <stdint.h>

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x03
#define FS_BLOCKDEVICE 0x04
#define FS_PIPE        0x05
#define FS_SYMLINK     0x06
#define FS_MOUNTPOINT  0x08 // Is the file an active mountpoint?

struct dirent // One of these is returned by the readdir call, according to POSIX.
{
  char name[128]; // Filename.
  uint32_t ino;     // Inode number. Required by POSIX.
};

typedef struct fs_node
{
   char name[128];     // The filename.
   uint32_t mask;        // The permissions mask.
   uint32_t uid;         // The owning user.
   uint32_t gid;         // The owning group.
   uint32_t flags;       // Includes the node type. See #defines above.
   uint32_t inode;       // This is device-specific - provides a way for a filesystem to identify files.
   uint32_t length;      // Size of the file, in bytes.
   uint32_t impl;        // An implementation-defined number.
   uint32_t (*read)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
   uint32_t (*write)(struct fs_node*, uint32_t, uint32_t, uint8_t*);
   void (*open)(struct fs_node*);
   void (*close)(struct fs_node*);
   struct dirent* (*readdir_type_t)(struct fs_node*, uint32_t);
   struct fs_node* (*finddir_type_t)(struct fs_node*, char *name);
   struct fs_node *ptr; // Used by mountpoints and symlinks.
} fs_node_t;

extern fs_node_t *fs_root; // The root of the filesystem.

// Standard read/write/open/close functions. Note that these are all suffixed with
// _fs to distinguish them from the read/write/open/close which deal with file descriptors
// not file nodes.
uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
void open_fs(fs_node_t *node, uint8_t read, uint8_t write);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, uint32_t index);
fs_node_t *finddir_fs(fs_node_t *node, char *name);

struct initramfs_file {
  char name[128];
  uint16_t flags;
  uint32_t size;
} __attribute__((packed));

struct initramfs_header {
  uint32_t magic; //0x45524653
  uint32_t count;
  struct initramfs_file files[0];
} __attribute__((packed));

void vfs_process(void* ptr);
struct initramfs_header* vfs_open();

#endif
