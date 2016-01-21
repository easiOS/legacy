#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <eelphant.h>
#include <video.h>
#include <vfs.h>

#define EFM_SELECTED_FILE 0

void efm_load(ep_window* w)
{
	strcpy(w->title, "Eelphant File Manager");
	w->x = vwidth() / 2 - 320;
	w->y = vheight() / 2 - 240;
	w->w = 640;
	w->h = 480;
	w->bg.r = 212;
	w->bg.g = 212;
	w->bg.b = 212;
	w->bg.a = 255;
	w->userdata[EFM_SELECTED_FILE] = 0;
}

void efm_update(uint64_t dt, ep_window* w)
{

}

void efm_draw(int64_t bx, int64_t by, ep_window* w)
{
	struct initramfs_header* vfs = vfs_open();
	void* f = vfs->files;
	int n = w->userdata[EFM_SELECTED_FILE];
	vsetcol(0, 0, 0, 255);
	vd_rectangle(FILL, bx + 5, by + 5 + 20 * n, 200, 25);
	for(int i = 0; i < vfs->count; i++)
	{
		if(i == n)
			vsetcol(255, 255, 255, 255);
		else
			vsetcol(0, 0, 0, 255);
		struct initramfs_file* file = f;
		char buffer[130];
		memset(buffer, 0, 130);
		strcat(buffer, "* ");
		strcat(buffer, file->name);
		strcat(buffer, "\t");
		char buffer2[32];
		itoa(file->size, buffer2, 10);
		strcat(buffer, buffer2);
		strcat(buffer, " bytes");
		vd_print(bx + 10, by + 10 + 20 * i, buffer, NULL, NULL);
		f += sizeof(struct initramfs_file) + file->size;
	}
}

void efm_event(struct keyevent* ke, struct mouseevent* me, ep_window* w)
{
	struct initramfs_header* vfs = vfs_open();
	if(ke)
	{
		if(ke->release)
		{
			switch(ke->keycode)
			{
				case 0x48: //cursor up
					if(w->userdata[EFM_SELECTED_FILE] > 0)
					{
						w->userdata[EFM_SELECTED_FILE]--;
					}
					else
					{
						w->userdata[EFM_SELECTED_FILE] = vfs->count - 1;
					}
				break;
				case 0x4d: //cursor down
					if(w->userdata[EFM_SELECTED_FILE] < vfs->count - 1)
					{
						w->userdata[EFM_SELECTED_FILE]++;
					}
					else
					{
						w->userdata[EFM_SELECTED_FILE] = 0;
					}
				break;
			}
		}
	}
}

void efm_spawn()
{
	ep_window* w = eelphant_create_window();
	if(!w)
	{
		printf("efm: can't allocate window\n");
		return;
	}
	w->load = &efm_load;
	w->draw = &efm_draw;
	w->load(w);
	eelphant_switch_active(w);
}