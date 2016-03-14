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
#include <dev/pci/debugdrv.h>
#include <kernel.h>
#include <dev/pci.h>
#include <acpi.h>
#include <krandom.h>
#include <fs/thinfat32.h>

#include "terminal.h"
#include "msgbox.h"
#include "eclock.h"
#include "notepad.h"
#include "physdemo.h"
#include "luavm.h"
#include "testapp.h"
#include "efm.h"
#include "login.h"

#define EP_MAX_WINDOWS 32

int32_t ep_mx = 20, ep_my = 20;
size_t ep_sw, ep_sh;
ep_window windows[EP_MAX_WINDOWS];
uint32_t date[6];
ep_window* window_active = NULL;
ep_window* window_active_cmd = NULL;
time_t last_frame_time = 60;
time_t frame_time = 60;
int ep_restart = 0;
int ep_cls = 1; //clear screen?
int ep_proc = 0; //window is running
int ep_locked = 0; //locked, user needs to login

char cmd_buf[64];
int cmd_buf_i = 0;
bool cmd_active = false;

int ep_tz = 0;

extern struct lua_apps lua_apps[16];

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

void eelphant_screenshot(void);

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
  //printf("CMDLEN: %d\n", cmdlen);
  char* token = strtok(cmd_copy, " ");
  while(token)
  {
    args[argc++] = token;
    token = strtok(NULL, " ");
  }
  //printf("CMD: %s\n", args[0]);
  #define CMDCMP(str) if(strcmp(args[0], str) == 0)
  CMDCMP("terminal")
  {
    terminal_spawn();
    return;
  }
  CMDCMP("efm")
  {
    efm_spawn();
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
    if(graphics_available())
    {
      puts("setting res...\n");
      //vbgfx_set_res(w, h);
      graphics_set_mode(w, h, 32);
      ep_restart = 1;
    }
    else
    {
      puts("resolution: no graphics card available\n");
    }
    return;
  }
  CMDCMP("reboot")
  {
    for(int y = 0; y < vheight(); y++)
    {
      for(int x = 0; x < vwidth(); x++)
      {
        vsetcol(krandom_get() % 256, krandom_get() % 256, krandom_get() % 256, 255);
        vplot_nb(x, y);
      }
    }
    reboot("Requested by user");
  }
  CMDCMP("lspci")
  {
    pci_ls();
    return;
  }
  CMDCMP("lsacpi")
  {
    struct rsdp_desc* rsdp_d = acpi_getrsdp();
    if(!rsdp_d)
    {
      puts("lsacpi: no ACPI\n");
      return;
    }
    puts("=========\n");
    puts("OEM ID: ");
    for(int i = 0; i < 6; i++) putc(rsdp_d->oemid[i]);
    putc('\n');
    puts("RSDT OEM ID: ");
    for(int i = 0; i < 6; i++) putc(rsdp_d->rsdt->OEMID[i]);
    putc('\n');
    puts("=========\n");
    return;
  }
  CMDCMP("lscpu")
  {
    puts("\n========\nlscpu\n");
    struct cpu_desc* cpud = get_cpu_desc();
    printf("CPU Vendor: %s\n", cpud->vendor);
    printf("CPU Brand: %s\n", cpud->brand);
    printf("Features: %d, %d\n", cpud->features1, cpud->features2);
    puts("========\n");
    return;
  }
  CMDCMP("eclock")
  {
    eclock_spawn();
    return;
  }
  CMDCMP("notepad")
  {
    notepad_spawn();
    return;
  }
  CMDCMP("pcidbg")
  {
    puts("pcidbg\n");
    if(argc < 2) return;
    int bus = atoi(args[1]);
    int slot = atoi(args[2]);
    pciddrvinit(bus, slot);
    return;
  }
  CMDCMP("phys") //we'll need a proper searching algorithm soon
  {
    puts("physics demo\n");
    physdemo_spawn();
    return;
  }
  CMDCMP("testapp")
  {
    testapp_init();
    return;
  }
  CMDCMP("kernupd")
  {
    kernupd_init();
    return;
  }
  puts("Searching on initrd...\n");
  for(int i = 0; i < 16; i++)
  {
    if(lua_apps[i].name[0] == '\0') continue;
    if(strcmp(lua_apps[i].name, args[0]) == 0)
    {
      luavm_spawn(lua_apps[i].address);
      return;
    }
  }
  puts("Searching on disk...\n");
  char buffer[256];
  memset(buffer, 0, 256);
  strcat(buffer, "/bin/");
  strcat(buffer, args[0]);
  TFFile* f = tf_fopen((uint8_t*)buffer, (const uint8_t*)'r');
  if(f)
  {
    printf("Executable found on disk!\n");
    uint8_t* app_addr = malloc(f->size);
    if(!app_addr)
    {
      printf("Failed to load application: out of memory!\n");
      tf_fclose(f);
      return;
    }
    tf_fread(app_addr, f->size, f);
    tf_fclose(f);
    luavm_spawn((lheader_t*)app_addr);
    free(app_addr);
    return;
  }
  printf("eelphant: command not found: %s\n", args[0]);
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
    if(window_active)
      if(window_active->event)
      {
        ep_proc = 1;
        window_active->event(ke, NULL, window_active);
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
          cmd_active = !cmd_active;
          if(cmd_active)
          {
            window_active_cmd = window_active;
            window_active = NULL;
          }
          else
          {
            window_active = window_active_cmd;
          }
          break;
        case 0x1C: //enter
          if(ep_locked) break;
          if(cmd_active)
          {
            eelphant_eval(cmd_buf);
            cmd_buf_i = 0;
            memset(cmd_buf, 0, 64);
            cmd_active = false;
          }
          break;
        case 0x0E: //backspace
          if(!cmd_active) break;
          cmd_buf_i = 0;
          memset(cmd_buf, 0, 64);
          break;
        case 0x01: //escape
          if(ep_locked)
          {
            reboot("User closed login window");
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
  }
}

void eelphant_update(time_t dt)
{
  frame_time = dt;
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
  get_time((uint32_t*)&date);
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
  vd_print(ep_sw - 190, ep_sh - 30, "EasiOS Professional", NULL, NULL);
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
      vd_print(w->x + 8, w->y - 20, w->title, NULL, NULL);
      vsetcol(212, 212, 204, 255);
      vd_rectangle(FILL, w->x+w->w - 32, w->y - 24, 32, 24);
      vsetcol(156, 156, 156, 255);
      vd_rectangle(LINE, w->x+w->w - 32, w->y - 24, 32, 24);
      vsetcol(0,0,0,255);
      vd_print(w->x+w->w - 20, w->y - 18, "X", NULL, NULL);
      //vsetcol(236, 240, 241, 255);
      vsetcol(w->bg.r, w->bg.g, w->bg.b, w->bg.a);
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
  if(cmd_active)
  {
    for(int i = 0; i < 512; i++)
    {
      vsetcol(149, 165, 166, 255 - i / 2);
      for(int j = 0; j < 29; j++)
        vplot(i, j);
    }
  }
  vsetcol(4, 4, 4, 255);
  int64_t datex, datey;
  char b[16];
  /*itoa(date[0], b, 10);
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
  vd_print(datex, datey, b, &datex, &datey);*/
  itoa((date[3] + ep_tz) % 24, b, 10);
  if(b[1] == '\0')
  {
    b[1] = b[0];
    b[0] = '0';
    b[2] = '\0';
  }
  vd_print(ep_sw - 75, 5, b, &datex, &datey);
  vd_print(datex, datey, ":", &datex, &datey);
  itoa(date[4], b, 10);
  if(b[1] == '\0')
  {
    b[1] = b[0];
    b[0] = '0';
    b[2] = '\0';
  }
  vd_print(datex, datey, b, &datex, &datey);
  //vsetcol(255, 0, 0, 150);
  //frame_time = (frame_time ? frame_time : 1);
  //vd_print(10, ep_sh - 32, itoa(1000 / frame_time, b, 10), &datex, &datey);
  //vd_print(datex, datey, " FPS - FRAME TIME: ", &datex, &datey);
  //vd_print(datex, datey, itoa(last_frame_time, b, 10), NULL, NULL);
  vsetcol(252, 252, 252, 255);
  vd_print(4, 10, cmd_buf, NULL, NULL);
  last_frame_time = frame_time;
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

int eelphant_main(int64_t width, int64_t height)
{
  puts("Eelphant Window Manager v0\n");
  ep_restart = 0;
  ep_sw = width;
  ep_sh = height;
  time_t last, now;
  last = ticks();
  now = last;
  lastmouse = time(NULL);
  TFFile* tzf = tf_fopen((uint8_t*)"/cfg/timezone", (const uint8_t*)"r");
  if(tzf)
  {
    char tzb[3];
    memset(tzb, 0, 3);
    tf_fread(tzb, 2, tzf);
    tf_fclose(tzf);
    ep_tz = atoi(tzb);
  }
  puts("Entering login loop\n");
  ep_locked = 1;
  ep_window* loginw = login_init();
  while(ep_locked)
  {
    now = ticks();
    struct keyevent* ke = kbdpoll();
    loginw->event(ke, NULL, loginw);
    loginw->update(now - last, loginw);
    loginw->draw(0, 0, loginw);
    vswap();
    last = now;
  }
  eelphant_destroy_window(loginw);
  printf("Entering user loop\n");
  ep_locked = 0;
  msgbox_show("Welcome to EasiOS Professional!\n\nPress ALT to open the command bar\nMove windows using keypad cursors\nUse TAB to switch between windows\nPress ESCAPE to close the current window\nThank you and have a productive day!", "EasiOS", NONE, 0);
  while(true)
  {
    last = now;
    now = ticks();
    eelphant_event(now - last);
    eelphant_draw();
    eelphant_update(now - last);
    vswap();
    /*draw_avg = (draw_avg + now - last) / 2;
    vsetcol(255, 0, 0, 255);
    vd_print(10, 400, itoa(draw_avg, b, 10), NULL, NULL);*/
    if(ep_restart)
    {
      return 1;
    }
  }
  return 0;
}

void eelphant_switch_active(ep_window* w)
{
  if(!w) return;
  window_active = w;
}

void eelphant_screenshot(void)
{
  extern uint32_t* fb;
  extern int64_t fbw, fbh;
  TFFile* f = tf_fopen((uint8_t*)"/user/scr.raw", (uint8_t*)"w");
  if(!f)
    return;
  tf_fwrite(fb, 4, fbw * fbh, f);
  tf_fclose(f);
  puts("Screenshot has been saved to /user/scr.raw\n");
}