#ifndef H_VFS
#define H_VFS

#include <stdint.h>
#include "video.h"
#include "timer.h"

#define VFS_MAX_SUBNODE 64

typedef struct node {
	char name[128];
	uint32_t mask, uid, flags, length;
	//read_type_t read;
	void* subnodes;
} vfs_node_t;

vfs_node_t* vfs_get_cnode();
void vfs_init();

vfs_node_t root;

#endif