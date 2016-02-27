#include <eelphant.h>
#include <string.h>
#include <stdlib.h>
#include <fs/thinfat32.h>
#include "iowindow.h"

void iowin_unload(ep_window* w)
{
	free((void*)w->userdata[0]);
}

void iowin_event(struct keyevent* ke, struct mouseevent* me, ep_window* w)
{
	char* b = (char*)w->userdata[0];
	int i = w->userdata[1];
	int mode = w->userdata[2];
	if(ke)
	{
		if(ke->character == '\n' && !ke->release)
		{
			char buffer[64];
			buffer[0] = '\0';
			strcat(buffer, "/user/");
			strcat(buffer, b);
			if(mode == 0)
			{
				TFFile* f = tf_fopen((uint8_t*)buffer, (uint8_t*)"r");
				if(f)
				{
					size_t a = w->userdata[4];
					tf_fread((void*)w->userdata[3], a, f);
					size_t* b = (size_t*)w->userdata[4];
					*b = f->size;
					printf("Cursor at %d\n", *(size_t*)w->userdata[4]);
				}
				tf_fclose(f);
				eelphant_switch_active((ep_window*)w->userdata[5]);
				eelphant_destroy_window(w);
				return;
			}
			if(mode == 1)
			{
				size_t a = w->userdata[4];
				TFFile* f = tf_fopen((uint8_t*)buffer, (uint8_t*)"w");
				if(f)
					tf_fwrite((void*)w->userdata[3], a, 1, f);
				tf_fclose(f);
				eelphant_switch_active((ep_window*)w->userdata[5]);
				eelphant_destroy_window(w);
				return;
			}
		}
		if((ke->character > 31 && ke->character < 128) && !ke->release)
		{
			if(i == 15) return;
			b[i++] = ke->character;
			b[i] = '\0';
		}
		if(ke->character == '\b' && !ke->release)
		{
			if(i == 0) return;
			b[--i] = '\0';
		}
	}
	w->userdata[1] = i;
}

void iowin_draw(int64_t bx, int64_t by, ep_window* w)
{
	char* b = (char*)(w->userdata[0]);
	vsetcol(255, 255, 255, 255);
	vd_rectangle(FILL, bx + 10, by + 40, 230, 20);
	vsetcol(156, 156, 156, 255);
	vd_rectangle(LINE, bx + 10, by + 40, 230, 20);
	vsetcol(0, 0, 0, 255);
	vd_print(bx + 10, by + 15, w->userdata[2] ? "Save as:" : "Open:", NULL, NULL);
	vd_print(bx + 10, by + 40, b, NULL, NULL);
}

void iowin_spawn(int mode, char* name, void* data, size_t* len, ep_window* parent)
{
	ep_window* w = eelphant_create_window();
  	if(!w) return;
  	strcpy(w->title, mode ? "Save file" : "Open file");
  	char* fnamebuf = malloc(16);
  	memset(fnamebuf, 0, 16);
  	if(name != NULL && strlen(name) < 15)
  	{
  		memcpy(fnamebuf, name, 8);
  	}
  	w->userdata[0] = (uint32_t)fnamebuf;
  	w->userdata[1] = 0;
  	w->userdata[2] = mode;
  	w->userdata[3] = (uint32_t)data;
  	w->userdata[4] = *len;
  	w->userdata[5] = (uint32_t)parent;
  	w->x = (parent != NULL) ? parent->x + 50 : 100;
  	w->y = (parent != NULL) ? parent->y + 50 : 100;
  	w->w = 250;
  	w->h = 100;
  	w->bg.r = 212;
  	w->bg.g = 212;
  	w->bg.b = 212;
  	w->bg.a = 255;
  	w->event = &iowin_event;
  	w->draw = &iowin_draw;
  	w->unload = &iowin_unload;
  	eelphant_switch_active(w);
}