#include <stdio.h>
#include <stdint.h>
#include <video.h>
#include <amethyst.h>
#include <stdlib.h>
#include <string.h>

struct meminsp_windata
{
	void* address;
	char size; // 1, 2 or 4 bytes
};

int meminsp_unload(am_win* w)
{
	return 0;
}

int meminsp_load(am_win* w)
{
	strcpy(w->title, "Memory inspector");
	w->x = 50;
	w->y = 50;
	w->w = 720;
	w->h = 480;
	w->bg.r = 212;
	w->bg.g = 212;
	w->bg.b = 212;
	w->bg.a = 255;
	return 0;
}

void meminsp_update(am_win* w, unsigned dt)
{

}

void meminsp_drawb(am_win* w, int bx, int by)
{
	vsetcol(0, 0, 0, 255);
	struct meminsp_windata* wd = w->windata;
	char buffer[2500];
	snprintf(buffer, 512, "Memory contents at 0x%x", wd->address);
	vd_print(bx + 10, by + 10, buffer, NULL, NULL);
	char buffer2[64];
	const char* newline = "\n";
	buffer[0] = '\0';
	unsigned char* mem = wd->address;
	for(int j = 0; j < 20; j++)
	{
		for(int i = 0; i < 20; i++)
		{
			snprintf(buffer2, 64, "%x ", mem[i]);
			strcat(buffer, buffer2);
		}
		mem += 20;
		strcat(buffer, newline);
	}
	vd_print(bx + 10, by + 40, buffer, NULL, NULL);
}

void meminsp_drawc(am_win* w, int bx, int by)
{
	vsetcol(0, 0, 0, 255);
	struct meminsp_windata* wd = w->windata;
	char buffer[1768];
	snprintf(buffer, 512, "Memory contents at 0x%x", wd->address);
	vd_print(bx + 10, by + 10, buffer, NULL, NULL);
	char buffer2[64];
	const char* newline = "\n";
	buffer[0] = '\0';
	unsigned char* mem = wd->address;
	for(int j = 0; j < 20; j++)
	{
		for(int i = 0; i < 20; i++)
		{
			char c = mem[i];
			if(c < 32 || c > 126)
				c = '.';
			snprintf(buffer2, 64, "%c ", c);
			strcat(buffer, buffer2);
		}
		mem += 20;
		strcat(buffer, newline);
	}
	vd_print(bx + 10, by + 40, buffer, NULL, NULL);
}

void meminsp_event(am_win* w, am_event* e)
{
	if(e->type != KEYRELEASE)
		return;
	struct meminsp_windata* wd = w->windata;
	switch(e->data.key.keycode)
	{
		case 0x49:
		case 0xC9:
		{
			wd->address += 800;
			break;
		}
		case 0x51:
		case 0xD1:
		{
			if(wd->address - 800 > wd->address) // to prevent overflow
			{
					wd->address = 0;
					return;
			}
			wd->address -= 800;
			break;
		}
		case 0x32:
		case 0xB2: // m released
		{
			w->draw = (w->draw == &meminsp_drawb) ? &meminsp_drawc : &meminsp_drawb;
			break;
		}
	}
}

int meminsp_spawn(int argc, char** argv)
{
	if(argc < 2)
	{
		printf("meminsp: not enough arguments, usage: %s [addr]\n", argv[0]);
		return 1;
	}
	am_win* w = amethyst_create_window();
	if(!w)
	{
		printf("meminsp: can't allocate window\n");
		return 1;
	}
	w->load = &meminsp_load;
	w->unload = &meminsp_unload;
	w->update = &meminsp_update;
	w->draw = &meminsp_drawb;
	w->event = &meminsp_event;
	w->load(w);
	w->windata = malloc(sizeof(struct meminsp_windata));
	struct meminsp_windata* wd = w->windata;
	wd->address = (void*)atou(argv[1]);
	wd->size = 1;
	amethyst_set_active(w);
	AM_WIN_SETKEYEV(w);
	return 0;
}