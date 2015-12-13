#ifndef H_EELPHANT
#define H_EELPHANT

#include <stdint.h>
#include <stddef.h>
#include <dev/kbd.h>
#include <dev/mouse.h>
#include <video.h>

typedef struct ep_window ep_window;

struct ep_window {
  int64_t x, y, w, h, z;
  char title[64];
  uint32_t flags;
  //flags:
  ///31-1 0
  ///rrrr F
  ///F - focused
  void (*load)(ep_window*); //init function(ptr to window)
  void (*update)(uint64_t, ep_window*); //update function (delta time, ptr to window)
  void (*draw)(uint64_t, uint64_t, ep_window*); //draw function (window coordinates, ptr to window)
  void (*event)(struct keyevent*, struct mouseevent*, ep_window*); //event handler function(ptr to keyevent, ptr to mouseevent, ptr to window)
  uint32_t userdata[64];
  rgb_t bg;
};

int eelphant_main(int64_t width, int64_t height);
ep_window* eelphant_create_window();
void eelphant_destroy_window(ep_window* w);
void eelphant_switch_active(ep_window* w);

#endif
