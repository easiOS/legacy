/* EasiOS amethyst/amethyst.c
 * ----------------
 * Author(s): Daniel (Easimer) Meszaros
 * ----------------
 * Description: Amethyst/Eelphant2 Window System
 */

#include <amethyst.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <dev/timer.h>
#include <dev/kbd.h>
#include <string.h>

#include "testwin.h"
#include "ifconfig.h"

am_win am_windows[AM_MAX_WINDOWS];
am_win* am_active = NULL;
unsigned am_date[6];
unsigned am_width, am_height;
unsigned am_flags;
int am_cls;
unsigned am_dt = 0;

int am_cmd = 0;
char am_cmd_buffer[64] = {'\0'};
int am_cmd_buffer_i = 0;
am_cmd_t* am_cmd_suggestions[4];
int am_cmd_suggestions_n = 0;

am_cmd_t am_commands[64] = {
	{
		.name = "testwin",
		.argc = 1,
		.main = &testwin_main,
	},
	{
		.name = "ifconfig",
		.argc = 1,
		.main = &ifconfig_main,
	},
	// End of Array
	{
		.name = "",
		.argc = 0,
		.main = NULL,
	} 
};

am_cmd_t* am_last = NULL;
am_cmd_t* am_last2 = NULL;

void amethyst_cmdeval(char* s)
{
	char* args[32] = { NULL };
	char* s1 = s;
	args[0] = s;
	unsigned argi = 1;
	for(int i = 0; i < 64; i++)
	{
		if(argi == 31)
			break;
		char c = *s1;
		if(c == ' ')
		{
			*s1 = '\0';
			args[argi] = s1 + 1;
			argi++;
		}
		s1++;
	}
	for(int i = 0; i < 64; i++)
	{
		if(strcmp(am_commands[i].name, args[0]) == 0)
		{
			int ret = 1;
			if(am_commands[i].main)
				ret = am_commands[i].main(argi, args);
			if(am_last == &am_commands[i])
				break;
			if(ret != 0)
			{
				printf("amethyst: spawner of %s exited with code %d\n", am_commands[i].name, ret);
			}
			am_last2 = am_last;
			am_last = &am_commands[i];
		}
	}
}

void amethyst_event()
{
	struct keyevent* ke = kbdpoll();
	while(ke)
	{
		am_event e;
		e.type = (ke->release) ? KEYRELEASE : KEYPRESS;
		e.data.key.keycode = ke->keycode;
		e.data.key.character = ke->character;
		if(ke->ctrl)
			AM_KEYEVENT_SETCTRL(&e);
		if(ke->shift)
			AM_KEYEVENT_SETSHIFT(&e);
		if(ke->alt)
			AM_KEYEVENT_SETALT(&e);
		if(ke->doublescan)
			AM_KEYEVENT_SETDUBS(&e);
		if(am_active && !am_cmd)
			if(AM_WIN_KEYEV(am_active))
				if(am_active->event)
				{
					am_active->event(am_active, &e);
				}
		// wm keys
		if(ke->release)
			switch(ke->keycode)
			{
				case 0x5B: //left GUI
				{
					am_cmd = !am_cmd;
					am_cls = 1;
					break;
				}
				case 0x0E: // backspace
				{
					if(!am_cmd) break;
          			am_cmd_buffer_i = 0;
          			am_cmd_buffer[0] = '\0';
          			break;
				}
				case 0x1C: // enter
				{
					if(!am_cmd) break;
					amethyst_cmdeval(am_cmd_buffer);
					am_cmd_buffer_i = 0;
          			am_cmd_buffer[0] = '\0';
          			am_cmd = 0;
          			am_cls = 1;
					break;
				}
				case 0x48: // cursor up
				{
					if(am_active)
					{
						am_active->y-= 10;
						am_cls = 1;
					}
					break;
				}
				case 0x4B: // cursor left
				{
					if(am_active)
					{
						am_active->x -= 10;
						am_cls = 1;
					}
					break;
				}
				case 0x4D: // cursor right
				{
					if(am_active)
					{
						am_active->x += 10;
						am_cls = 1;
					}
					break;
				}
				case 0x50: // cursor down
				{
					if(am_active)
					{
						am_active->y += 10;
						am_cls = 1;
					}
					break;
				}
				case 0x01: // escape
				{
					if(am_active)
					{
						amethyst_destroy_window(am_active);
						am_cls = 1;
					}
					break;
				}
				case 0x0f: // tab
				{
					for(int i = 0; i < AM_MAX_WINDOWS; i++)
						if(am_windows[i].flags & 1)
						{
							am_active = &am_windows[i];
							am_cls = 1;
							break;
						}
					break;
				}
				default:
				{
					if(am_cmd)
						if(am_cmd_buffer_i < 63)
						{
							am_cmd_buffer[am_cmd_buffer_i] = ke->character;
							am_cmd_buffer[am_cmd_buffer_i++ + 1] = '\0';
							// search suggestions
							am_cmd_suggestions_n = 0;
							memset(am_cmd_suggestions, 0, 4 * sizeof(am_cmd_t*));
							for(int i = 0; i < 64; i++)
							{
								if(am_cmd_suggestions_n >= 4)
									break;
								am_cmd_t* c = &am_commands[i];
								if(!c->argc)
									break;
								for(int j = 0; j < 64; j++)
								{
									if(am_cmd_buffer[j] == '\0')
									{
										am_cmd_suggestions[am_cmd_suggestions_n++] = c;
										break;
									}
									if(am_cmd_buffer[j] != c->name[j])
										break;
								}

							}
						}
					break;
				}
			}
		ke = kbdpoll();
	}
}

void amethyst_update(unsigned dt)
{
	am_dt = dt;
	if(am_active)
		if(am_active->update)
			am_active->update(am_active, dt);
	get_time((uint32_t*)am_date);
}

void amethyst_draw()
{
	// clear screen if needed
	if(am_cls)
	{
		puts("amethyst: cleared screen\n");
		vsetcol(60, 108, 164, 255);
		vcls();
		am_cls = 0;
	}
	vsetcol(255,255,255,255);
  	vd_print(am_width - 190, am_height - 55, "EasiOS Professional", NULL, NULL);
  	// draw windows
	for(int i = 0; i < AM_MAX_WINDOWS; i++)
	{
		if(&am_windows[i] == am_active)
			continue;
		am_win* w = &am_windows[i];
		if(!w->draw) // window has no draw function -> maybe it's a background process
			continue;
		// draw background
		vsetcol(w->bg.r, w->bg.g, w->bg.r, 255);
		vd_rectangle(FILL, w->x, w->y - 32, w->w, w->h + 32);
		w->draw(w, w->x, w->y);
		// draw frame
		vsetcol(138, 138, 117, 255);
		vd_rectangle(LINE, w->x, w->y, w->w, w->h);
		// draw titlebar
		vsetcol(12, 37, 108, 255);
		vd_rectangle(FILL, w->x + 1, w->y - 31, w->w, 31);
		vsetcol(138, 138, 117, 255);
		vd_rectangle(LINE, w->x, w->y - 32, w->w, 32);
		vsetcol(255, 255, 255, 255);
		vd_print(w->x + 8, w->y - 28, w->title, NULL, NULL);
	}
	if(am_active)
		if(am_active->draw)
		{
			// draw background
			vsetcol(am_active->bg.r, am_active->bg.g, am_active->bg.r, 255);
			vd_rectangle(FILL, am_active->x, am_active->y - 32, am_active->w, am_active->h + 32);
			// draw window
			am_active->draw(am_active, am_active->x, am_active->y);
			// draw frame
			vsetcol(138, 138, 117, 255);
			vd_rectangle(LINE, am_active->x, am_active->y, am_active->w, am_active->h);
			// draw titlebar
			vsetcol(12, 37, 108, 255);
			vd_rectangle(FILL, am_active->x + 1, am_active->y - 31, am_active->w, 31);
			vsetcol(138, 138, 117, 255);
			vd_rectangle(LINE, am_active->x, am_active->y - 32, am_active->w, 32);
			vsetcol(255, 255, 255, 255);
			vd_print(am_active->x + 8, am_active->y - 28, am_active->title, NULL, NULL);
		}
	// draw wm widgets
	vsetcol(212, 212, 204, 255);
	vd_rectangle(FILL, 1, am_height - 31, am_width - 2, 31);
	vsetcol(252, 252, 252, 255);
	vd_rectangle(LINE, 0, am_height - 32, am_width, 32);
	/// draw time
	char datebuf[64] = {'\0'};
	snprintf(datebuf, 64, "%d:%d", am_date[3], am_date[4]);
	vsetcol(0, 0, 0, 255);
	vd_print(am_width - 56, am_height - 26, datebuf, NULL, NULL);
	// draw command window (if active)
	if(!am_cmd)
		return;
	vsetcol(212, 212, 204, 255);
	vd_rectangle(FILL, 1, am_height - 287, 255, 255);
	vsetcol(252, 252, 252, 255);
	vd_rectangle(LINE, 0, am_height - 286, 256, 256);
	vsetcol(255, 255, 255, 255);
	vd_rectangle(FILL, 4, am_height - 56, 248, 26);
	if(am_cmd_buffer_i)
	{
		vsetcol(0, 0, 0, 255);
		vd_print(6, am_height - 52, am_cmd_buffer, NULL, NULL);
	}
	if(am_last)
	{
		vsetcol(0, 0, 0, 255);
		vd_line(4, am_height - 276, 252, am_height - 276);
		vd_print(6, am_height - 274, "Recent", NULL, NULL);
		vd_print(6, am_height - 250, am_last->name, NULL, NULL);
		if(am_last2)
			vd_print(6, am_height - 226, am_last2->name, NULL, NULL);
	}
	if(am_cmd_suggestions_n)
	{
		vsetcol(0, 0, 0, 255);
		vd_line(4, am_height - 200, 252, am_height - 200);
		vd_print(6, am_height - 198, "Suggestions", NULL, NULL);
		for(int i = 0; i < am_cmd_suggestions_n; i++)
		{
			if(!am_cmd_suggestions[i])
				continue;
			vd_print(6, am_height - 174 + (i * 24), am_cmd_suggestions[i]->name, NULL, NULL);
		}
	}
}

// timer callback, called every tick
void amethyst_event_tick(unsigned time)
{
	am_event e;
	e.type = TICK;
	e.data.tick.time = time;
	if(am_active)
		if(AM_WIN_TICKEV(am_active))
			if(am_active->event)
			{
				am_active->event(am_active, &e);
			}
}

int amethyst_main(int width, int height)
{
	puts("Amethyst Window Manager\n");
	if(width < 1024 || height < 768)
	{
		printf("Amethyst cannot be run at this resolution (%dx%d), at least 1024x768 is required.\n", width, height);
		return 0;
	}

	am_width = width;
	am_height = height;
	am_flags = 0;
	am_cls = 1;
	memset(am_windows, 0, AM_MAX_WINDOWS * sizeof(am_win));

	time_t last, now;
  	last = ticks();
  	now = last;
  	//int timerid = timer_registercb(&amethyst_event_tick, 1);
  	puts("amethyst: entering loop\n");
	while(1)
	{
		last = now;
    	now = ticks();
    	amethyst_event();
		amethyst_update(now - last);
		amethyst_draw();
		vswap();
	}

	//timer_unregistercb(timerid);

	return 0;
}

am_win* amethyst_create_window()
{
  for(int i = 0; i < AM_MAX_WINDOWS; i++)
  {
    if(!(am_windows[i].flags & 1))
    {
      am_windows[i].flags = 1;
      return &am_windows[i];
    }
  }
  return NULL; //no free window slot
}

void amethyst_destroy_window(am_win* w)
{
  if(!w) return;
  if(w->unload) w->unload(w);
  w->flags = 0;
  w->w = 0;
  w->h = 0;
  w->x = 0;
  w->y = 0;
  w->z = 0;
  w->title[0] = '\0';
  w->load = NULL;
  w->update = NULL;
  w->draw = NULL;
  w->event = NULL;
  w->unload = NULL;
  am_cls = 1;
}

void amethyst_set_active(am_win* w)
{
	am_active = w;
}