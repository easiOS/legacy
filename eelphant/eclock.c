#include <eelphant.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <video.h>
#include <dev/timer.h>
#include <math.h>
#include "eclock.h"

void eclock_load(ep_window* w)
{
  get_time(w->userdata);
}

void eclock_update(uint64_t dt, ep_window* w)
{
  get_time(w->userdata);
  w->w = 500;
  w->h = 500;
}

void eclock_draw(int64_t bx, int64_t by, ep_window* w)
{
  vsetcol(0, 0, 0, 255);
  vd_circle(bx + 250, by + 250, 225);
  int x = 212, y = 112;
  for(int i = 0; i < 12; i++)
  {
    vplot(x, y);
    //x += 
  }
}

void eclock_event(struct keyevent* ke, struct mouseevent* me, ep_window* w)
{

}

void eclock_spawn()
{
  ep_window* w = eelphant_create_window();
  if(!w) return;
  strcpy(w->title, "eclock");
  w->x = 50;
  w->y = 50;
  w->w = 500;
  w->h = 500;
  w->load = &eclock_load;
  w->update = &eclock_update;
  w->draw = &eclock_draw;
  w->event = &eclock_event;
  w->z = 0;
  w->bg.r = 255;
  w->bg.g = 255;
  w->bg.b = 255;
  w->bg.a = 255;
  w->load(w);
  eelphant_switch_active(w);
}
