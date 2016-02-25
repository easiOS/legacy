#include <eelphant.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "msgbox.h"

const uint16_t warn_icon[] = {
  0b0000000000000000,
  0b0000000000000000,
  0b0000000000000000,
  0b0000000110000000,
  0b0000001111000000,
  0b0000011001100000,
  0b0000111001110000,
  0b0000111001110000,
  0b0001111001111000,
  0b0001111111111000,
  0b0011111001111100,
  0b0011111001111100,
  0b0111111111111110,
};

void msgbox_load(ep_window* w)
{

}

void msgbox_unload(ep_window* w)
{
  free((void*)w->userdata[2]);
}

void msgbox_update(uint64_t dt, ep_window* w)
{

}

void msgbox_draw(int64_t bx, int64_t by, ep_window* w)
{
  switch(w->userdata[0])
  {
    case WARN:
      vsetcol(0,0,0,255);
      vd_rectangle(FILL, bx + 12, by + 25, 2, 7);
      vsetcol(254, 254, 0, 255);
      vd_bitmap16((uint16_t*)&warn_icon, bx + 5, by + 20, 13);
      break;
  }
  vsetcol(4,4,4,255);
  vd_print(bx + 25, by + 20, (char*)&(w->userdata[2]), NULL, NULL);
  if(w->userdata[1] == 1)
  {
    vsetcol(212, 212, 204, 255);
    vd_rectangle(FILL, bx + w->w / 2 - 20, by + w->h * 3/4, 40, 20);
    vsetcol(132, 132, 132, 255);
    vd_rectangle(LINE, bx + w->w / 2 - 20, by + w->h * 3/4, 40, 20);
    vsetcol(4,4,4,255);
    vd_print(bx + w->w / 2 - 15, by + w->h * 3/4, "OK", NULL, NULL);
  }
}

void msgbox_event(struct keyevent* ke, struct mouseevent* me, ep_window* w)
{
  if(ke->keycode == 0x1C && !ke->release)
  {
    eelphant_destroy_window(w);
  }
}

int msgbox_show(const char* message, const char* title, enum MsgBoxType type, unsigned int buttons)
{
  ep_window* w = eelphant_create_window();
  if(!w) return -1;
  strcpy(w->title, title);
  w->x = 200; w->y = 200;
  w->h = 50;
  //longest line length in message
  int msglen = strlen(message);
  int linemaxlen = 0;
  int j = 0;
  for(int i = 0; i < msglen; i++)
  {
    if(message[i] == '\n')
    {
      if(j > linemaxlen) linemaxlen = j;
      j = 0;
      continue;
    }
    if(message[i] == '\0')
    {
      if(j > linemaxlen) linemaxlen = j;
      break;
    }
    j++;
  }
  w->w = linemaxlen * 12 + 30;
  w->z = 1000;
  w->unload = &msgbox_unload;
  w->load = &msgbox_load;
  w->update =  &msgbox_update;
  w->draw = &msgbox_draw;
  w->event = &msgbox_event;
  w->userdata[0] = type;
  w->userdata[1] = buttons;
  char* msg = (char*)&w->userdata[2];
  msglen = msglen > 250 ? 250 : msglen;
  for(int i = 0; i < msglen; i++)
  {
    msg[i] = message[i];
    if(msg[i] == '\n') w->h += 16;
  }
  msg[250] = '\0';
  w->bg.r = 212;
  w->bg.g = 212;
  w->bg.b = 212;
  w->bg.a = 255;
  w->load(w);
  eelphant_switch_active(w);
  return 0;
}
