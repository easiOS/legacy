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
  char asd[81];
  for(int i = 0; i < 25; i++)
  {
    memset(asd, 0, 81);
    for(int j = 0; j < 80; j++)
    {
      asd[j] = ((uint16_t*)w->userdata[0])[i * 80 + j] & 0b1111111;
    }
    vd_print(bx + 5, by + i * 16, asd, NULL, NULL);
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
