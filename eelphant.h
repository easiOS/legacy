#ifndef H_EELPHANT
#define H_EELPHANT

#include <stdint.h>
#include <stddef.h>
#include "gba.h"

typedef struct ep_window ep_window;

typedef struct {
  enum {MOVE, CLICK, KEY} type;
  int16_t mx, my;
  int16_t key;
} event_t;

struct ep_window {
  int16_t x, y, w, h, z;
  char title[32];
  uint16_t flags;
  //flags:
  ///15-1 0
  ///rrrr F
  ///F - focused
  void (*load)(ep_window*); //init function(ptr to window)
  void (*unload)(ep_window*); //destructor(ptr to window)
  void (*update)(uint64_t, ep_window*); //update function (delta time, ptr to window)
  void (*draw)(int64_t, int64_t, ep_window*); //draw function (window coordinates, ptr to window)
  void (*event)(event_t* event, ep_window*); //event handler function(ptr to event, ptr to window)
  uint16_t userdata[16];
  gba_pixel bg;
};

int eelphant_main(int16_t width, int16_t height);
ep_window* eelphant_create_window();
void eelphant_destroy_window(ep_window* w);
void eelphant_switch_active(ep_window* w);

#endif /* H_EELPHANT */