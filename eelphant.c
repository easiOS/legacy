/* EasiOS kernel.c
 * ----------------
 * Author(s): Daniel (Easimer) Meszaros
 * ----------------
 * Description: Eelphant Window System
 */

#include "eelphant.h"
#include "video.h"
#include <stdlib.h>
#include <stdbool.h>

#define EP_MAX_WINDOWS 4

size_t ep_sw, ep_sh;
ep_window windows[EP_MAX_WINDOWS];
uint32_t date[6];
ep_window* window_active = NULL;
ep_window* window_active_cmd = NULL;
int ep_restart = 0;
int ep_cls = 1; //clear screen?
int ep_proc = 0; //window is running
int ep_locked = 0; //locked, user needs to login
int16_t ep_mx, ep_my;

const uint16_t cursor[] = {
  0b1111111100000000,
  0b1111111100000000,
  0b1111111000000000,
  0b1111100000000000,
  0b1111110000000000,
  0b1110111000000000,
  0b1110011100000000,
  0b1100001100000000
};

void eelphant_event(unsigned dt)
{
  return;
  /*while(ke != NULL)
  {
    if(window_active)
      if(window_active->event)
      {
        ep_proc = 1;
        window_active->event(NULL, window_active);
        ep_proc = 0;
      }
    if(!ke->release) //is pressed
    {
      if(ke->ctrl)
      {
      }
      if(ke->shift) //is shift
      {
      }
      if(ke->ctrl && ke->shift)
      {
      }
      switch(ke->keycode)
      {
        case 0x48: //keypad 8 (up)
        {
          if(window_active)
          {
            window_active->y -= 25;
            ep_cls = 1;
          }
          break;
        }
        case 0x4b: //keypad 4 (left)
        {
          if(window_active)
          {
            window_active->x -= 25;
            ep_cls = 1;
          }
          break;
        }
        case 0x50: //keypad 2 (right)
        {
          if(window_active)
          {
            window_active->y += 25;
            ep_cls = 1;
          }
          break;
        }
        case 0x4d: //keypad 6 (down)
        {
          if(window_active)
          {
            window_active->x += 25;
            ep_cls = 1;
          }
          break;
        }
        case 0x0F: //tab
            for(int i = 0; i < EP_MAX_WINDOWS; i++)
            {
              windows[i].z = 10;
              if(&windows[i] != window_active && windows[i].flags & 1)
              {
                windows[i].z = 1;
                window_active = &windows[i];
                break;
              }
            }
          break;
        case 0x5B: //left GUI
          if(ep_locked) break;
          break;
        case 0x1C: //enter
          if(ep_locked) break;
          break;
        case 0x0E: //backspace
          if(!cmd_active) break;
          break;
        case 0x01: //escape
          if(ep_locked)
          {
            //reboot("User closed login window");
            break;
          }
          if(window_active)
          {
            eelphant_destroy_window(window_active);
            window_active = NULL;
            for(int i = 0; i < EP_MAX_WINDOWS; i++)
            {
              if(windows[i].flags & 1)
              {
                window_active = &windows[i];
              }
            }
          }
          break;
        case 0x2A:
        case 0xE0:
        case 0x37:
        {
          if(ke->doublescan)
          {
            eelphant_screenshot();
          }
          break;
        }
        default:
          if(cmd_active)
            if(cmd_buf_i < 64)
              if(!ke->release)
                if(ke->character)
                  cmd_buf[cmd_buf_i++] = ke->character;
          break;
      }
    }
    ke = kbdpoll();
  }*/
}

void eelphant_update(unsigned dt)
{
  /*for(int i = 0; i < 16; i++)
  {
    if(windows[i].flags & 1)
    {
      if(windows[i].update)
        windows[i].update((uint64_t)dt, &windows[i]);
    }
  }*/
  if(window_active)
    if(window_active->update)
    {
      ep_proc = 1;
      window_active->update(dt, window_active);
      ep_proc = 0;
    }
  /*if(windows[0].flags >> 6 & 1)
  {
    windows[0].x+=10 * (dt / 10);
  }
  if(!(windows[0].flags >> 6 & 1))
  {
    windows[0].x-=10 * (dt / 10);
  }
  if(windows[0].x >= 300 && (windows[0].flags >> 6 & 1))
  {
    windows[0].flags &= ~(1 << 6);
  }
  if(windows[0].x <= 100 && !(windows[0].flags >> 6 & 1))
  {
    windows[0].flags |= 1 << 6;
  }*/
  //get_time((uint32_t*)&date);
}

void eelphant_draw()
{
  //draw desktop
  vsetcol(60, 108, 164, 255);
  if(ep_cls)
  {
    vcls();
    ep_cls = 0;
  }
  vsetcol(255,255,255,255);
  //vd_print(ep_sw - 190, ep_sh - 30, "EasiOS Professional", NULL, NULL);
  //draw windows
  int draw_order[EP_MAX_WINDOWS];
  for(int i = 0; i < EP_MAX_WINDOWS; i++) draw_order[i] = i;
  //order window drawing
  int begin = 0;
  int end = EP_MAX_WINDOWS-1;
  bool swapped;
  do
  {
    swapped = false;
    for(int i = begin; i < end; i++)
    {
      if(windows[draw_order[i]].z < windows[draw_order[i+1]].z)
      {
        int tmp = draw_order[i];
        draw_order[i] = draw_order[i+1];
        draw_order[i+1] = tmp;
        swapped = true;
      }
    }
    end--;
    if(!swapped) break;
    swapped = false;
    for(int i = end; i > begin; i--)
    {
      if(windows[draw_order[i]].z > windows[draw_order[i-1]].z)
      {
        int tmp = draw_order[i];
        draw_order[i] = draw_order[i-1];
        draw_order[i-1] = tmp;
        swapped = true;
      }
    }
    begin++;
  } while(swapped);
  for(int i = 0; i < EP_MAX_WINDOWS; i++)
  {
    ep_window* w = &windows[draw_order[i]];
    if(w->flags & 1 && w->w > 0 && w->h > 0) //active
    {
      if(w == window_active)
        vsetcol(12, 37, 108, 255);
      else
        vsetcol(189, 195, 199, 200);
      vd_rectangle(FILL, w->x, w->y - 24, w->w, 24);
      vsetcol(156, 156, 156, 255);
      vd_rectangle(LINE, w->x, w->y - 24, w->w, 24);
      vsetcol(255, 255, 255, 255);
      //vd_print(w->x + 8, w->y - 20, w->title, NULL, NULL);
      vsetcol(212, 212, 204, 255);
      vd_rectangle(FILL, w->x+w->w - 32, w->y - 24, 32, 24);
      vsetcol(156, 156, 156, 255);
      vd_rectangle(LINE, w->x+w->w - 32, w->y - 24, 32, 24);
      vsetcol(0,0,0,255);
      //vd_print(w->x+w->w - 20, w->y - 18, "X", NULL, NULL);
      //vsetcol(236, 240, 241, 255);
      vsetcolw(w->bg, 255);
      vd_rectangle(FILL, w->x, w->y, w->w, w->h);
      if(w->draw && w == window_active)
      {
        ep_proc = 1;
        w->draw(w->x, w->y, w);
        ep_proc = 0;
      }
    }
  }
  //draw cursor
  //if(time(NULL) - lastmouse < 2)
  vsetcol(26, 188, 156, 255);
  vd_rectangle(FILL, ep_mx, ep_my, 16, 16);
  vsetcol(22, 160, 133, 255);
  vd_rectangle(LINE, ep_mx, ep_my, 16, 16);
  //vsetcol(22, 160, 133, 255);
  //vd_bitmap16(cursor, ep_mx, ep_my, 8);
  //draw command bar
  vsetcol(212, 212, 204, 255);
  vd_rectangle(FILL, 0, 0, 1024, 28);
  vsetcol(252, 252, 252, 255);
  vd_rectangle(LINE, 0, 0, 1024, 28);
  vsetcol(4, 4, 4, 255);
}

ep_window* eelphant_create_window()
{
  for(int i = 0; i < EP_MAX_WINDOWS; i++)
  {
    if(!(windows[i].flags & 1))
    {
      windows[i].flags = 1;
      return &windows[i];
    }
  }
  return NULL; //no free window slot
}

void eelphant_destroy_window(ep_window* w)
{
  if(!w) return;
  if(w->unload) w->unload(w);
  w->flags = 0;
  w->w = 0;
  w->h = 0;
  w->x = 0;
  w->y = 0;
  w->z = 0;
  w->title[0] = '\0';
  w->load = NULL;
  w->update = NULL;
  w->draw = NULL;
  w->event = NULL;
  w->unload = NULL;
  ep_cls = 1;
}

int draw_avg = 0;

int eelphant_main(int16_t width, int16_t height)
{
  ep_restart = 0;
  ep_sw = width;
  ep_sh = height;
  ep_mx = 0; ep_my = 0;
  ep_window* w = eelphant_create_window();
  vsetcol(212, 212, 212, 255);
  w->bg = vgetcol();
  w->x = 10;
  w->y = 75;
  w->w = 150;
  w->h = 100;
  while(true)
  {
    eelphant_event(0);
    eelphant_draw();
    eelphant_update(0);
    /*draw_avg = (draw_avg + now - last) / 2;
    vsetcol(255, 0, 0, 255);
    vd_print(10, 400, itoa(draw_avg, b, 10), NULL, NULL);*/
    if(ep_restart)
    {
      return 1;
    }
    while((*(volatile uint16_t*)0x04000006) != 160);
  }
  return 0;
}

void eelphant_switch_active(ep_window* w)
{
  if(!w) return;
  window_active = w;
}