#include "loginwin.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <video.h>
#include <users.h>

//userdata:
//0 - pointer to username
//1 - pointer to password
//2 - username index
//3 - password index
//4 - return value
//5 - login state

enum LoginState {
  NAME, PASSWORD, CHECKING
};

void login_load(ep_window* w)
{
  w->userdata[0] = (uint32_t)malloc(sizeof(char)*64);
  w->userdata[1] = (uint32_t)malloc(sizeof(char)*64);
  w->userdata[2] = 0;
  w->userdata[3] = 0;
  w->userdata[5] = NAME;
}

void login_unload(ep_window* w)
{
  if((char*)w->userdata[0] != NULL)
    free((char*)w->userdata[0]);
  if((char*)w->userdata[1] != NULL)
    free((char*)w->userdata[1]);
}

void login_draw(int64_t bx, int64_t by, ep_window* w)
{
  vd_print(bx + 20, by + 20, (char*)w->userdata[0], NULL, NULL);
  vd_print(bx + 20, by + 40, (char*)w->userdata[1], NULL, NULL);
}

void login_event(struct keyevent* ke, struct mouseevent* me, ep_window* w)
{
  if(ke)
  {
    if(!ke->release)
    {
      if(ke->keycode == 0x1C)
      {
        switch(w->userdata[5])
        {
          case NAME:
          {
            w->userdata[5] = PASSWORD;
            break;
          }
          case PASSWORD:
          {
            w->userdata[5] = CHECKING;
            if(eos_users_auth((char*)w->userdata[0], (char*)w->userdata[1]) == 0)
            {
              w->userdata[4] = 0x1234C0CA;
            }
            else
            {
              memset((char*)w->userdata[0], 0, 64);
              memset((char*)w->userdata[1], 0, 64);
              w->userdata[5] = NAME;
            }
            break;
          }
        }
      }
      else if(ke->character > 31)
      {
        switch(w->userdata[5])
        {
            case NAME:
            {
              if(w->userdata[2] < 64)
              {
                ((char*)w->userdata[0])[w->userdata[2]++] = ke->character;
              }
              break;
            }
            case PASSWORD:
            {
              break;
            }
            case CHECKING:
            {
              break;
            }
        }
      }
    }
  }
}

void login_update(uint64_t dt, ep_window* w)  {}

ep_window* eelphant_spawn_loginwin()
{
  ep_window* w = eelphant_create_window();
  if(!w) return NULL;
  w->draw = login_draw;
  w->event = login_event;
  w->load = login_load;
  w->unload = login_unload;
  w->update = login_update;
  w->w = 400;
  w->h = 250;
  w->bg.r = 212;
  w->bg.g = 212;
  w->bg.b = 204;
  w->bg.a = 255;
  w->x = vgetw() / 2 - (w->w / 2);
  w->y = vgeth() / 2 - (w->h / 2);
  w->load(w);
  eelphant_switch_active(w);
  return w;
}
