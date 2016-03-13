#include <eelphant.h>
#include <video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fs/thinfat32.h>
#include "notepad.h"
#include "iowindow.h"
#include "msgbox.h"

void notepad_draw(int64_t bx, int64_t by, ep_window* w)
{
  vsetcol(255, 255, 255, 255);
  vd_rectangle(FILL, bx + 2, by + 5, w->w - 4, w->h - 5);
  vsetcol(0, 0, 0, 255);
  vd_printl(bx + 2, by + 2, (char*)w->userdata[0], 80, 25);
}

void notepad_event(struct keyevent* ke, struct mouseevent* me, ep_window* w)
{
  if(ke)
  {
    if(ke->ctrl && !ke->release)
    {
      switch(ke->character)
      {
        case 'i':
        {
          if(w->userdata[1] > 80)
            w->userdata[1] -= 80;
          break;
        }
        case 'j':
        {
          w->userdata[1]--;
          break;
        }
        case 'k':
        {
          w->userdata[1] += 80;
          break;
        }
        case 'l':
        {
          w->userdata[1]++;
          break;
        }
        case 's':
        {
          size_t len = strlen((char*)w->userdata[0]);
          iowin_spawn(IOWIN_MODE_SAVE, NULL, (void*)w->userdata[0], &len, w);
          break;
        }
        case 'o':
        {
          size_t len = 65535;
          iowin_spawn(IOWIN_MODE_OPEN, NULL, (void*)w->userdata[0], &len, w);
          break;
        }
      }
      return;
    }
    if(((ke->character > 31 && ke->character < 128) || ke->character == '\n') && !ke->release)
    {
      char* b = (char*)w->userdata[0];
      int i = w->userdata[1];
      b[i] = ke->character;
      b[i+1] = '\0';
      w->userdata[1]++;
    }
    if(ke->character == '\b' && !ke->release)
    {
      char* b = (char*)w->userdata[0];
      int i = w->userdata[1];
      if(w->userdata[1] != 0)
        w->userdata[1] -= 1;
      b[i] = '\0';
    }
  }
}

void notepad_unload(ep_window* w)
{
  free((void*)w->userdata[0]);
}

ep_window* notepad_spawn()
{
  ep_window* w = eelphant_create_window();
  if(!w) return NULL;
  strcpy(w->title, "Notepad");
  char* buffer = (char*)malloc(65536);
  if(!buffer)
  {
    puts("notepad: out of memory\n");
    eelphant_destroy_window(w);
    return NULL;
  }
  buffer[0] = '\0';
  w->userdata[0] = (uint32_t)buffer;
  w->userdata[1] = 0;
  w->x = vwidth() / 2 - 450;
  w->y = vheight() / 2 - 240;
  w->w = 900;
  w->h = 480;
  w->bg.r = 212;
  w->bg.g = 212;
  w->bg.b = 212;
  w->bg.a = 255;
  w->draw = &notepad_draw;
  w->event = &notepad_event;
  w->unload = &notepad_unload;
  eelphant_switch_active(w);
  return w;
}

ep_window* notepad_spawnf(const char* fn)
{
  ep_window* w = notepad_spawn();
  uint8_t* buffer = (uint8_t*)w->userdata[0];
  TFFile* f = tf_fopen((uint8_t*)fn, (const uint8_t*)"r");
  if(!f)
  {
    msgbox_show("File not found!", "Error", ERR, OK);
    return w;
  }
  tf_fread(buffer, 65536, f);
  w->userdata[1] = f->size;
  tf_fclose(f);
  return w;
}