#include "vfs.h"

vfs_node_t* current;

vfs_node_t* vfs_get_cnode()
{
	return current;
}

void vfs_init()
{
	terminal_prfxi(ticks(), "Initializing VFS\n");
	root.name = "/";
	current = &root;
	vfs_node_t nullf;
	nullf.name = "null";
	((vfs_node_t* )root.subnodes)[0] = nullf;
	((vfs_node_t* )root.subnodes)[1] = nullf;
	terminal_prfx("VFS", " Working directory: ");
	terminal_writestring(current->name);
	terminal_putchar('\n');
	terminal_prfx("VFS", " Files:\n");
	vfs_node_t* nd;
	for(int i = 0; i < VFS_MAX_SUBNODE; i++)
	{
		nd = ((vfs_node_t**)current->subnodes)[i];
		if(nd != NULL)
		{
			terminal_writestring(nd->name);
			terminal_putchar(' ');
		}
	}
	terminal_putchar('\n');
}