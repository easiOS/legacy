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
#include <mouse.h>
#include <stdio.h>
#include <timer.h>

int32_t ep_mx = 20, ep_my = 20;
size_t ep_sw, ep_sh;
ep_window windows[16];

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

void eelphant_event()
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
}

void eelphant_update(time_t dt)
{

}

void eelphant_draw()
{
  //draw desktop
  //draw command bar
  vsetcol(149, 165, 166, 255);
  vcls();
  vsetcol(52, 73, 94, 255);
  vd_rectangle(FILL, 0, 0, 1024, 28);
  vsetcol(44, 62, 80, 255);
  vd_rectangle(LINE, 0, 0, 1024, 28);
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
      if(windows[draw_order[i]].z > windows[draw_order[i+1]].z)
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
      if(windows[draw_order[i]].z < windows[draw_order[i-1]].z)
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
      vsetcol(44, 62, 80, 255);
      vd_rectangle(FILL, w->x, w->y - 32, w->w, 32);
      vsetcol(52, 73, 94, 255);
      vd_rectangle(LINE, w->x, w->y - 32, w->w, 32);
      vsetcol(231, 76, 60, 255);
      vd_rectangle(FILL, w->x+w->w - 32, w->y, 32, 32);
      vsetcol(192, 57, 43, 255);
      vd_rectangle(LINE, w->x+w->w - 32, w->y, 32, 32);
      vsetcol(236, 240, 241, 255);
      vd_rectangle(FILL, w->x, w->y, w->w, w->h);
    }
  }
  //draw cursor
  //if(time(NULL) - lastmouse < 2)
  /*vsetcol(26, 188, 156, 255);
  vd_rectangle(FILL, ep_mx, ep_my, 16, 16);
  vsetcol(22, 160, 133, 255);
  vd_rectangle(LINE, ep_mx, ep_my, 16, 16);*/
  //vsetcol(22, 160, 133, 255);
  //vd_bitmap16(cursor, ep_mx, ep_my, 8);
  vswap();
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

void eelphant_main(size_t width, size_t height)
{
  puts("Eelphant Window Manager v0\n");
  memset(windows, 0, 16*sizeof(ep_window));
  ep_sw = width;
  ep_sh = height;
  time_t last, now;
  last = ticks();
  lastmouse = time(NULL);
  ep_window* test = eelphant_create_window();
  test->x = 20;
  test->y = 70;
  test->w = 640;
  test->h = 480;
  strcpy(test->title, "Test Window");
  puts("Entering loop\n");
  while(true)
  {
    now = ticks();
    eelphant_event();
    eelphant_update(now - last);
    eelphant_draw();
    last = now;
  }
}
