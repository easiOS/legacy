#include <amethyst.h>
#include <video.h>
#include <kernel.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include "about.h"

const char* about_text = KERNEL_NAME "\n\
Copyright (c) Easimer Labs 2015-2016  All Right Reserved\n\n\
Special thanks to the osdev.org community. Without their help,\n\
this window on your screen wouldn't be here.\n";

void about_draw(am_win* w, int bx, int by)
{
	vsetcol(0, 0, 0, 255);
	vd_print(bx + 10, by + 50, about_text, NULL, NULL);
}

int about_main(int argc, char** argv)
{
	am_win* w = amethyst_create_window();
	if(!w)
	{
		printf("about: can't allocate window\n");
		return 1;
	}
	strncpy(w->title, "About easiOS", 64);
	w->x = 300;
	w->y = 300;
	w->w = 720;
	w->h = 320;
	w->bg.r = 212;
	w->bg.g = 212;
	w->bg.b = 212;
	w->bg.a = 255;
	w->draw = &about_draw;
	amethyst_set_active(w);
	return 0;
}