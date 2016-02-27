#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <eelphant.h>
#include <video.h>
#include <vfs.h>
#include <dev/disk.h>
#include <fs/thinfat32.h>
#include <fs/thinternal.h>

#define EFM_SELECTED_FILE 0

extern struct eos_drives drives[4];

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
	for(int i = 0; i < 4; i++)
	{
		/*if(drives[i].type == 1)
		{
			struct initramfs_header* vfs = vfs_open();
			void* f = vfs->files;
			int n = w->userdata[EFM_SELECTED_FILE];
			vsetcol(0, 0, 0, 255);
			vd_rectangle(FILL, bx + i * 250 + 5, by + 5 + 20 * n, 250, 25);
			for(int j = 0; j < vfs->count; j++)
			{
				if(j == n)
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
				vd_print(bx + i*50 + 10, by + 10 + 20 * j, buffer, NULL, NULL);
				f += sizeof(struct initramfs_file) + file->size;
			}
		}
		else */if(drives[i].type == 2)
		{
		}
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
	for(int i = 0; i < 4; i++)
	{
		if(drives[i].letter == 0) continue;
		printf("Drive %c:\\ Size: %d MB %s\n", drives[i].letter, drives[i].type == 1 ? 0 : (drives[i].address.phys.size * 512 / 1024 / 1024), (drives[i].type == 0 ? "Physical" : (drives[i].type == 2 ? "Physical FAT32" : "Initrd")));
		if(drives[i].type == 2)
		{
			if(tf_fopen((uint8_t*)"/cfg/exist", (const uint8_t*)"w"))
			{
				printf("System partition detected\n");
			}

		}
	}
	eelphant_switch_active(w);
}