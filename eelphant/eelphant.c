/* EasiOS kernel.c
 * ----------------
 * Author(s): Daniel (Easimer) Meszaros
 * ----------------
 * Description: Eelphant Window System
 */

#include <eelphant.h>
#include <video.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dev/mouse.h>
#include <stdio.h>
#include <dev/timer.h>
#include <vm.h>
#include <dev/kbd.h>
#include <text.h>
#include <dev/pci/virtboxgfx.h>
#include <kernel.h>
#include <dev/pci.h>

#include "terminal.h"

int32_t ep_mx = 20, ep_my = 20;
size_t ep_sw, ep_sh;
ep_window windows[16];
uint32_t date[6];
ep_window* window_active;
time_t last_frame_time = 60;
time_t frame_time = 60;
int ep_restart = 0;

char cmd_buf[64];
int cmd_buf_i = 0;
bool cmd_active = false;

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

time_t lastmouse = 0;

void eelphant_eval(char* cmd)
{
  char* args[8];
  int argc = 0;
  char cmd_copy[64];
  memcpy(cmd_copy, cmd, 64);
  int cmdlen = -1;
  for(int i = 0; cmd_copy[i] != '\0'; cmdlen = i++);
  cmdlen++;
  char b[64];
  puts("CMDLEN: "); puts(itoa(cmdlen, b, 10));putc('\n');
  char* token = strtok(cmd_copy, " ");
  while(token)
  {
    args[argc++] = token;
    token = strtok(NULL, " ");
  }
  #define CMDCMP(str) if(strcmp(args[0], str) == 0)
  CMDCMP("terminal")
  {
    terminal_spawn();
    return;
  }
  CMDCMP("resolution")
  {
    if(argc < 2) return;
    int w = 0, h = 0;
    w = atoi(args[1]);
    h = atoi(args[2]);
    puts(itoa(w, b, 10)); putc(' '); puts(itoa(h, b, 10)); putc('\n');
    if(w == 0 || h == 0) return;
    if(vbgfx_isinit())
    {
      puts("setting res...\n");
      //vbgfx_set_res(w, h);
      graphics_set_mode(w, h, 32);
      ep_restart = 1;
    }
    else
    {
      puts("eelphant: `resolution\' only works in VirtualBox, QEMU or Bochs\n");
    }
    return;
  }
  CMDCMP("reboot")
  {
    for(int i = 0; i < 256; i+=32)
    {
      vsetcol(44, 62, 80, i);
      vcls();
      vswap();
    }
    reboot("Requested by user");
  }
  CMDCMP("lspci")
  {
    pci_ls();
  }
}

void eelphant_event(time_t dt)
{
  mousegetcoords(&ep_mx, &ep_my);
  if(false)
  {
    lastmouse = time(NULL);
    struct mouseevent* e = mousepoll();
    ep_mx += e->dx;
    ep_my += e->dy;
    if(ep_mx < 0) ep_mx = 0;
    if(ep_my < 0) ep_my = 0;
    if(ep_mx >= ep_sw) ep_mx = ep_sw - 1;
    if(ep_my >= ep_sh) ep_my = ep_sh - 1;
  }
  struct keyevent* ke = kbdpoll();
  while(ke != NULL)
  {
    for(int i = 0; i < 16; i++)
    {
      if(windows[i].flags & 1)
      {
        if(windows[i].event)
          windows[i].event(ke, NULL, &windows[i]);
      }
    }
  if(ke && !ke->release)
  {
    if(ke->shift)
    {
      switch(ke->keycode)
      {
        case 0x48:
          if(window_active)
          {
            window_active->h-=(dt>window_active->h ? window_active->h : dt);
          }
          break;
        case 0x4B:
          if(window_active)
          {
            window_active->w-=(dt>window_active->w ? window_active->w : dt);
          }
          break;
        case 0x4D:
          if(window_active)
          {
            window_active->w+=(dt>window_active->w ? window_active->w : dt);
          }
          break;
        case 0x50:
          if(window_active)
          {
            window_active->h+=(dt>window_active->h ? window_active->h : dt);;
          }
          break;
      }
    } else if(ke->ctrl) {
      switch(ke->keycode)
      {
        default:
        break;
      }
    } else
    switch(ke->keycode)
    {
      case 0x0F: //tab
          for(int i = 0; i < 16; i++)
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
      case 0x38: //left alt (cmd)
        cmd_active = !cmd_active;
        break;
      case 0x48: //cursor up
        if(window_active)
        {
          window_active->y-=(dt>window_active->h ? window_active->h : dt);
        }
        break;
      case 0x4B: //cursor left
        if(window_active)
        {
          window_active->x-= (dt>window_active->w ? window_active->w : dt);
        }
        break;
      case 0x4D: //cursor right
        if(window_active)
        {
          window_active->x+= (dt>window_active->w ? window_active->w : dt);;
        }
        break;
      case 0x50: //cursor down
        if(window_active)
        {
          window_active->y+= (dt>window_active->h ? window_active->h : dt);;
        }
        break;
      case 0x1C: //enter
        eelphant_eval(cmd_buf);
        cmd_buf_i = 0;
        memset(cmd_buf, 0, 64);
        cmd_active = false;
        break;
      case 0x0E: //backspace
        cmd_buf_i = 0;
        memset(cmd_buf, 0, 64);
        break;
      case 0x01: //escape
        if(window_active)
        {
          eelphant_destroy_window(window_active);
          window_active = NULL;
          for(int i = 0; i < 16; i++)
          {
            if(windows[i].flags & 1)
            {
              window_active = &windows[i];
            }
          }
        }
        break;
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
  }
}

void eelphant_update(time_t dt)
{
  frame_time = dt;
  for(int i = 0; i < 16; i++)
  {
    if(windows[i].flags & 1)
    {
      if(windows[i].update)
        windows[i].update((uint64_t)dt, &windows[i]);
    }
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
  get_time((uint32_t*)&date);
}

void eelphant_draw()
{
  //draw desktop
  vsetcol(149, 165, 166, 255);
  vcls();
  //draw windows
  int draw_order[16];
  for(int i = 0; i < 16; i++) draw_order[i] = i;
  //order window drawing
  int begin = 0;
  int end = 15;
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
  for(int i = 0; i < 16; i++)
  {
    ep_window* w = &windows[draw_order[i]];
    if(w->flags & 1 && w->w > 0 && w->h > 0) //active
    {
      if(w == window_active)
        vsetcol(44, 62, 80, 255);
      else
        vsetcol(189, 195, 199, 200);
      vd_rectangle(FILL, w->x, w->y - 32, w->w, 32);
      vsetcol(52, 73, 94, 255);
      vd_rectangle(LINE, w->x, w->y - 32, w->w, 32);
      vsetcol(236, 240, 241, 255);
      vd_print(w->x + 8, w->y - 26, w->title, NULL, NULL);
      vsetcol(231, 76, 60, 255);
      vd_rectangle(FILL, w->x+w->w - 32, w->y - 32, 32, 32);
      vsetcol(192, 57, 43, 255);
      vd_rectangle(LINE, w->x+w->w - 32, w->y - 32, 32, 32);
      //vsetcol(236, 240, 241, 255);
      vsetcol(w->bg.r, w->bg.g, w->bg.b, w->bg.a);
      vd_rectangle(FILL, w->x, w->y, w->w, w->h);
      if(w->draw)
      {
        w->draw(w->x, w->y, w);
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
  vsetcol(52, 73, 94, 255);
  vd_rectangle(FILL, 0, 0, 1024, 28);
  vsetcol(44, 62, 80, 255);
  vd_rectangle(LINE, 0, 0, 1024, 28);
  if(cmd_active)
  {
    for(int i = 0; i < 512; i++)
    {
      vsetcol(149, 165, 166, 255 - i / 2);
      for(int j = 0; j < 29; j++)
        vplot(i, j);
    }
  }
  vsetcol(236, 240, 241, 255);
  int64_t datex, datey;
  char b[16];
  itoa(date[0], b, 10);
  vd_print(ep_sw - 250, 5, b, &datex, &datey);
  vd_print(datex, datey, "/", &datex, &datey);
  itoa(date[1], b, 10);
  vd_print(datex, datey, b, &datex, &datey);
  vd_print(datex, datey, "/", &datex, &datey);
  itoa(date[2], b, 10);
  vd_print(datex, datey, b, &datex, &datey);
  vd_print(datex, datey, " ", &datex, &datey);
  itoa(date[3], b, 10);
  vd_print(datex, datey, b, &datex, &datey);
  vd_print(datex, datey, ":", &datex, &datey);
  itoa(date[4], b, 10);
  vd_print(datex, datey, b, &datex, &datey);
  vd_print(datex, datey, ":", &datex, &datey);
  itoa(date[5], b, 10);
  vd_print(datex, datey, b, &datex, &datey);
  vsetcol(255, 0, 0, 150);
  frame_time = (frame_time ? frame_time : 1);
  vd_print(10, ep_sh - 32, itoa(1000 / frame_time, b, 10), &datex, &datey);
  vd_print(datex, datey, " FPS - FRAME TIME: ", &datex, &datey);
  vd_print(datex, datey, itoa(last_frame_time, b, 10), NULL, NULL);
  vsetcol(44, 62, 80, 255);
  vd_print(4, 10, cmd_buf, NULL, NULL);
  vswap();
  last_frame_time = frame_time;
}

ep_window* eelphant_create_window()
{
  for(int i = 0; i < 16; i++)
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
}

int eelphant_main(int64_t width, int64_t height)
{
  puts("Eelphant Window Manager v0\n");
  ep_restart = 0;
  //memset(windows, 0, 16*sizeof(ep_window));
  ep_sw = width;
  ep_sh = height;
  time_t last, now;
  last = ticks();
  lastmouse = time(NULL);
  /*ep_window* test = eelphant_create_window();
  test->x = 20;
  test->y = 70;
  test->w = 640;
  test->h = 480;
  window_active = test;
  strcpy(test->title, "Test Window");*/
  puts("Entering loop\n");
  while(true)
  {
    now = ticks();
    eelphant_event(now - last);
    eelphant_update(now - last);
    eelphant_draw();
    last = now;
    if(ep_restart)
    {
      return 1;
    }
  }
  return 0;
}

void eelphant_switch_active(ep_window* w)
{
  puts("switching window..\n");
  window_active = w;
}
