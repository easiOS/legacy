#include <stdio.h>
#include <stdint.h>
#include <video.h>
#include <string.h>
#include <amethyst.h>

int testwin_unload(am_win* w)
{
	return 0;
}

int testwin_load(am_win* w)
{
	strcpy(w->title, "Amethyst Test Window");
	w->x = vwidth() / 2 - 320;
	w->y = vheight() / 2 - 240;
	w->w = 720;
	w->h = 480;
	w->bg.r = 212;
	w->bg.g = 212;
	w->bg.b = 212;
	w->bg.a = 255;
	printf("testwin: flags: %x\n", w->flags);
	//AM_WIN_SETKEYEV(w); // enable key events
	printf("testwin: flags: %x\n", w->flags);
	return 0;
}

void testwin_update(am_win* w, unsigned dt)
{
}

void testwin_draw(am_win* w, int bx, int by)
{
	
}

void testwin_event(am_win* w, am_event* e)
{
	puts("testwin: event received\n");
}

int testwin_main(int argc, char** argv)
{
	am_win* w = amethyst_create_window();
	if(!w)
	{
		printf("testwin: can't allocate window\n");
		return 1;
	}
	w->load = &testwin_load;
	w->unload = &testwin_unload;
	w->update = &testwin_update;
	w->draw = &testwin_draw;
	w->event = &testwin_event;
	w->load(w);
	amethyst_set_active(w);
	puts("testwin: spawned\n");
	return 0;
}