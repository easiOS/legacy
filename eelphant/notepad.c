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
  vd_rectangle(FILL, p->x, p->y, p->w, p->h);
  vsetcol(p->fg.r, p->fg.g, p->fg.b, p->fg.a);
  vd_print(p->x + 2, p->y + 2, p->buffer, NULL, NULL);
}

void notepad_draw(int64_t bx, int64_t by, ep_window* w)
{
  struct e_textbox_prop p;
  p.wh = w;
  p.x = bx + 2;
  p.y = by + 5;
  p.w = w->w - 4;
  p.h = w->h - 5;
  p.buffer = (char*)w->userdata[0];
  p.bufferlen = 1024;
  p.bg.r = 255;
  p.bg.g = 255;
  p.bg.b = 255;
  p.bg.a = 255;
  p.fg.r = 0;
  p.fg.g = 0;
  p.fg.b = 0;
  p.fg.a = 255;
  e_textbox_draw(&p);
}

void notepad_event(struct keyevent* ke, struct mouseevent* me, ep_window* w)
{
  if(ke)
  {
    if((ke->character > 31 && ke->character < 128 && !ke->release) || ke->character == '\n' || ke->character == '\b')
    {
      char* b = (char*)w->userdata[0];
      int i = w->userdata[1];
      putc(ke->character);
      b[i % 3] = ke->character;
      w->userdata[1] += 1;
      printf("notepad: buffer at 0x%x\n", w->userdata[0]);
      for(int j = 0; b[j] != '\0'; j++)
      {
        printf("%x ", b[j]);
      }
      putc('\n');
    }
  }
}

void notepad_unload(ep_window* w)
{
  free((void*)w->userdata[0]);
}

void notepad_spawn()
{
  ep_window* w = eelphant_create_window();
  if(!w) return;
  strcpy(w->title, "Notepad");
  char* buffer = (char*)(malloc(65536) + 16);
  if(!buffer)
  {
    puts("notepad: out of memory\n");
    eelphant_destroy_window(w);
    return;
  }
  printf("notepad: buffer at 0x%x\n", buffer);
  memset(buffer, 0, 1024); putc('\n');
  w->userdata[0] = (uint32_t)buffer;
  w->userdata[1] = 0;
  w->x = vwidth() / 2 - 320;
  w->y = vheight() / 2 - 240;
  w->w = 640;
  w->h = 480;
  w->bg.r = 212;
  w->bg.g = 212;
  w->bg.b = 212;
  w->bg.a = 255;
  w->draw = &notepad_draw;
  w->event = &notepad_event;
  w->unload = &notepad_unload;
  eelphant_switch_active(w);
}
