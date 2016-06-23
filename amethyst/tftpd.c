#include <stdio.h>
#include <stdint.h>
#include <video.h>
#include <amethyst.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <net/udp.h>

#include "tftpd.h"

enum tftpd_status
{
	UNBOUND, BOUND, DISABLED, ENABLED, READY, LOCKED
};

const char* tftpd_strings[] = {
	"Bounding to port 69...", "Bound to port!", "Disabled", "Waiting for connection...",
	"Client connected", "LOCKED"
};

const rgb_t tftpd_statuscolors[] = {
	{125, 125, 125, 255}, {150, 150, 150, 255},
	{175, 175, 175, 255}, {180, 180, 180, 255},
	{0, 255, 0, 255}, {255, 0, 0, 255}
};

struct tftpd_windata 
{
	enum tftpd_status status;
	unsigned char srcaddr[4];
};

int tftpd_unload(am_win* w)
{
	return 0;
}

int tftpd_load(am_win* w)
{
	strcpy(w->title, "TFTPd Control Panel");
	w->x = vwidth() / 2 - 320;
	w->y = vheight() / 2 - 240;
	w->w = 360;
	w->h = 240;
	w->bg.r = 212;
	w->bg.g = 212;
	w->bg.b = 212;
	w->bg.a = 255;
	return 0;
}

void tftpd_recv(void* p, void* data, unsigned len, uint16_t* srca, uint16_t* dsta, uint16_t src, uint16_t dst)
{
	am_win* w = p;
	struct tftpd_windata* wd = w->windata;
	unsigned short opcode = *(unsigned short*)data;
	if(!opcode || opcode > TFTP_ERR)
		return;
	if(opcode < 3)
	{

	}
}

void tftpd_update(am_win* w, unsigned dt)
{
	struct tftpd_windata* wd = w->windata;
	switch(wd->status)
	{
		case UNBOUND:
		{
			if(udp_bindport(69, tftpd_recv, w))
			{
				printf("tftpdcp: cannot bind to port 69\n");
				amethyst_destroy_window(w);
			}
			else
				wd->status = BOUND;
			break;
		}
		case BOUND:
		{
			wd->status = DISABLED;
			break;
		}
		default:
		{
			break;
		}
	}	
}

void tftpd_draw(am_win* w, int bx, int by)
{
	struct tftpd_windata* wd = w->windata;
	rgb_t bc = tftpd_statuscolors[wd->status];
	vsetcol(bc.r, bc.g, bc.b, 255);
	vd_rectangle(FILL, bx + 30, by + 100, 300, 50);
	vsetcol(0, 0, 0, 255);
	vd_print(bx + 40, by + 110, tftpd_strings[wd->status], NULL, NULL);
	vd_print(bx + 40, by + 170, "Press SPACE to disable/enable tftpd", NULL, NULL);
}

void tftpd_event(am_win* w, am_event* e)
{
	if(e->type != KEYRELEASE)
		return;
	struct tftpd_windata* wd = w->windata;
	if(e->data.key.character == ' ')
	{
		if(wd->status == ENABLED || wd->status == DISABLED)
			wd->status = (wd->status == DISABLED) ? ENABLED : DISABLED;
	}
}

int tftpd_spawn(int argc, char** argv)
{
	am_win* w = amethyst_create_window();
	if(!w)
	{
		printf("tftpd: can't allocate window\n");
		return 1;
	}
	w->load = &tftpd_load;
	w->unload = &tftpd_unload;
	w->update = &tftpd_update;
	w->draw = &tftpd_draw;
	w->event = &tftpd_event;
	w->load(w);
	w->windata = malloc(sizeof(struct tftpd_windata));
	struct tftpd_windata* wd = w->windata;
	wd->status = 0;
	AM_WIN_SETKEYEV(w);
	amethyst_set_active(w);
	return 0;
}