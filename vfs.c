#include "vfs.h"

vfs_node_t* current;

vfs_node_t* vfs_get_cnode()
{
	return current;
}

void vfs_init()
{
	terminal_prfx(ticks(), "Initializing VFS\n");
	root.name[0] = '/';
	root.name[1] = '\0';
	current = &root;
}