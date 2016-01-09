#include "physdemo.h"
#include <stdint.h>
#include <stddef.h>
#include <eelphant.h>
#include <video.h>
#include <math.h>
#include <string.h>

#define T 0
#define SX0 1
#define SY0 2
#define VX 3
#define VY 4
#define SX 5
#define SY 6

void physdemo_load(ep_window* w)
{
  float* ud = (float*)w->userdata;
  ud[T] = 0.0;
  ud[SX0] = 50.0;
  ud[SY0] = 100.0;
  ud[VX] = 15.0;
  ud[VY] = 20.0;
  ud[SX] = 50.0;
  ud[SY] = 100.0;
}

void physdemo_update(uint64_t dt, ep_window* w)
{
  if(!(w->flags >> 12 & 1)) return;
  float* ud = (float*)w->userdata;
  if(ud[SY] <= 30)
  {
    w->flags &= ~(1 << 12);
    return;
  }
  ud[T] += (float)dt / 20.0;
  ud[SX] = ud[SX0] + ud[VX] * ud[T];
  ud[SY] = ud[SY0] + ud[VY] * ud[T] + -1.0 * ud[T] * ud[T];
}

void physdemo_draw(int64_t bx, int64_t by, ep_window* w)
{
  float* ud = (float*)w->userdata;
  vsetcol(255, 0, 0, 255);
  if(!(w->flags >> 12 & 1))
  {
    vd_print(bx + 10, by + 10, "Press SPACE to start!", NULL, NULL);
  }
  vd_line(bx, by + w->h - 10, bx + w->w, by + w->h - 10);
  vd_circle(bx + ud[SX], by + w->h - ud[SY], 20);
}

void physdemo_event(struct keyevent* ke, struct mouseevent* me, ep_window* w)
{
  if(!ke) return;
  if(ke->character == ' ' && ke->release)
  {
    w->flags |= 1 << 12;
    w->load(w);
  }
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
  w->bg.r = 0;
  w->bg.g = 0;
  w->bg.b = 0;
  w->bg.a = 255;
  w->load(w);
  eelphant_switch_active(w);
}
