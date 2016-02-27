#include <eelphant.h>
#include <string.h>
#include <video.h>
#include <stdlib.h>
#include <config.h>
#include <kernel.h>
#include "login.h"

extern int ep_locked;

void login_update(uint64_t dt, ep_window* w)
{

}

void login_unload(ep_window* w)
{
	free((void*)w->userdata[0]);
}

void login_draw(int64_t bx, int64_t by, ep_window* w)
{
	vsetcol(0, 78, 152, 255);
	vd_rectangle(FILL, 0, 0, 1024, 100);
	vd_rectangle(FILL, 0, 668, 1024, 100);
	if(w->userdata[2])
	{
		vsetcol(255, 255, 255, 255);
		vd_rectangle(FILL, 400, 250, 400, 30);
		vsetcol(156, 156, 156, 255);
		vd_rectangle(LINE, 400, 250, 400, 30);
		char buffer[128];
		for(int i = 0; i < 128; i++)
		{
			if(((char*)w->userdata[0])[i] != 0)
			{
				buffer[i] = '*';
			}
			else
			{
				buffer[i] = 0;
				break;
			}
		}
		vsetcol(255, 255, 255, 255);
		vd_print(400, 200, "Password:", NULL, NULL);
		vsetcol(0, 0, 0, 255);
		vd_print(405, 255, buffer, NULL, NULL);
	}
	else
	{
		vsetcol(255, 255, 255, 255);
		vd_print(400, 200, "Press Ctrl-Shift-L to begin.", NULL, NULL);
	}
	vsetcol(255, 255, 255, 255);
	vd_print(64, 600, "Press Escape to restart the computer.", NULL, NULL);
}

void login_event(struct keyevent* ke, struct mouseevent* me, ep_window* w)
{
	if(ke && ke->keycode == 0x01)
		reboot("Requested by user");
	if(ke && !w->userdata[2])
	{
		if(ke->ctrl && ke->shift && ke->release)
		{
			if(ke->character == 'l' || ke->character == 'L')
			{
				w->userdata[2] = 1;
				vsetcol(60, 108, 164, 255);
				vcls();
			}
		}
	}
	if(ke && w->userdata[2])
	{
		if(ke->character == '\n' && !ke->release)
		{
			if(strcmp(((char*)w->userdata[0]), USER_PASSWORD) == 0)
				ep_locked = 0;
			else
			{
				((char*)w->userdata[0])[0] = '\0';
				w->userdata[1] = 0;
			}
		}
		else if(ke->character > 31 && ke->character < 128 && !ke->release)
		{
			((char*)w->userdata[0])[w->userdata[1]++] = ke->character;
			((char*)w->userdata[0])[w->userdata[1]] = '\0';
			if(w->userdata[1] > 510)
			{
				w->userdata[1] = 510;
			}
		}
		else if(ke->character == '\b' && !ke->release)
		{
			((char*)w->userdata[0])[w->userdata[1]--] = '\0';	
			if(w->userdata[1] < 0)
			{
				w->userdata[1] = 0;
			}
		}
	}
}

ep_window* login_init(void)
{
	#ifndef USER_PASSWORD
	ep_locked = 0;
	return NULL;
	#endif
	if(strlen(USER_PASSWORD) == 0)
	{
		ep_locked = 0;
		return NULL;
	}
	ep_window* w = NULL;
	w = eelphant_create_window();
	w->w = 1024;
	w->h = 768;
	w->x = 0;
	w->y = 0;
	w->bg.r = 212;
	w->bg.g = 212;
	w->bg.b = 212;
	w->bg.a = 255;
	w->update = &login_update;
	w->unload = &login_unload;
	w->draw = &login_draw;
	w->event = &login_event;
	w->userdata[0] = (uint32_t)malloc(512);
	w->userdata[1] = 0;
	w->userdata[2] = 0;

	((char*)w->userdata[0])[0] = '\0';
	vsetcol(60, 108, 164, 255);
	vcls();
	eelphant_switch_active(w);
	return w;
}