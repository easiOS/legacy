#include <stdio.h>
#include <stdint.h>
#include <video.h>
#include <amethyst.h>

int app_unload(am_win* w)
{
	
}

int app_load(am_win* w)
{
	strcpy(w->title, "Amethyst Application Template");
	w->x = vwidth() / 2 - 320;
	w->y = vheight() / 2 - 240;
	w->w = 720;
	w->h = 480;
	w->bg.r = 212;
	w->bg.g = 212;
	w->bg.b = 212;
	w->bg.a = 255;
}

void app_update(am_win* w, unsigned dt)
{

}

void app_draw(am_win* w, int bx, int by)
{
	
}

void app_event(am_win* w, am_event* e)
{
	
}

int app_spawn(int argc, char** argv)
{
	am_win* w = amethyst_create_window();
	if(!w)
	{
		printf("app: can't allocate window\n");
		return;
	}
	w->load = &app_load;
	w->unload = &app_unload;
	w->update = &app_update;
	w->draw = &app_draw;
	w->event = &app_event;
	w->load(w);
	amethyst_set_active(w);
}