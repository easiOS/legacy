#include <eelphant.h>
#include <video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "notepad.h"

struct e_textbox_prop
{
  ep_window* wh;
  int x, y;
  int w, h;
  char* buffer;
  size_t bufferlen;
  rgb_t bg, fg;
};

void e_textbox_draw(struct e_textbox_prop* p)
{
  vsetcol(p->bg.r, p->bg.g, p->bg.b, p->bg.a);
  vd_rectangle(FILL, p->wh->x + p->x, p->wh->y + p->y, p->w, p->h);
}

void notepad_spawn()
{
  ep_window* w = eelphant_create_window();
  if(!w) return;
  strcpy(w->title, "Notepad");
  char* buffer = (char*)malloc(1024);
  if(!buffer)
  {
    puts("notepad: out of memory\n");
    eelphant_destroy_window(w);
    return;
  }
  w->x = 10;
  w->y = 10;
  w->w = 640;
  w->h = 480;
  w->bg.r = 212;
  w->bg.g = 212;
  w->bg.b = 212;
  w->bg.a = 255;
  eelphant_switch_active(w);
}
