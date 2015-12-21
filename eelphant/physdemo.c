#include "physdemo.h"
#include <stdint.h>
#include <stddef.h>
#include <eelphant.h>
#include <video.h>

#define T 0
#define SX0 1
#define SY0 2
#define VX 3
#define VY 4
#define SX 5
#define SY 6

void physdemo_load(ep_window* w)
{
  w->userdata[T] = 0;
  w->userdata[SX0] = 10;
  w->userdata[SY0] = 20;
  w->userdata[VX] = 9;
  w->userdata[VY] = 5;
  w->userdata[SX] = 0;
  w->userdata[SY] = 0;
}

void physdemo_update(uint64_t dt, ep_window* w)
{
  w->userdata[T] += dt;
}

void physdemo_draw(uint64_t bx, uint64_t by, ep_window* w)
{

}

void physdemo_event(struct keyevent* ke, struct mouseevent* me, ep_window* w)
{

}

void physdemo_spawn()
{
  ep_window* w = eelphant_create_window();
  if(!w) return;
  strcpy(w->title, "Physics demo");
  w->x = 250;
  w->y = 250;
  w->w = 500;
  w->h = 500;
  w->load = &physdemo_load;
  w->update = &physdemo_update;
  w->draw = &physdemo_draw;
  w->event = &physdemo_event;
  w->z = 0;
  w->bg.r = 212;
  w->bg.g = 212;
  w->bg.b = 212;
  w->bg.a = 255;
  w->load(w);
  eelphant_switch_active(w);
}
