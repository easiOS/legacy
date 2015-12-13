#include <eelphant.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <video.h>
#include <text.h>

void terminal_load(ep_window* w)
{
  w->userdata[0] = (uint32_t)tgetbuf();
}

void terminal_update(uint64_t dt, ep_window* w)
{

}

void terminal_draw(uint64_t bx, uint64_t by, ep_window* w)
{
  vsetcol(192, 192, 192, 255);
  char asd[2];
  int64_t lx = bx, ly = by;
  for(int i = 0; i < 2000; i++)
  {
    if((i + 1) % 80 == 0 && i != 0){ ly += 16; lx = bx; }
    asd[0] = ((uint16_t*)(w->userdata[0]))[i] & 0b1111111;
    if(asd[0] < 32 || asd[0] > 127) continue;
    vd_print(lx, ly, asd, &lx, &ly);
  }
}

void terminal_event(struct keyevent* ke, struct mouseevent* me, ep_window* w)
{

}

void terminal_spawn()
{
  ep_window* w = eelphant_create_window();
  if(!w) return;
  strcpy(w->title, "Terminal");
  w->x = 50;
  w->y = 50;
  w->w = 750;
  w->h = 500;
  w->load = &terminal_load;
  w->update = &terminal_update;
  w->draw = &terminal_draw;
  w->event = &terminal_event;
  w->z = 0;
  w->bg.r = 0;
  w->bg.g = 0;
  w->bg.b = 0;
  w->bg.a = 255;
  w->load(w);
  eelphant_switch_active(w);
}
