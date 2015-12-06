#ifndef H_EELPHANT
#define H_EELPHANT

#include <stdint.h>
#include <stddef.h>

typedef struct {
  size_t x, y, w, h, z;
  char title[64];
  uint32_t flags;
  //flags:
  ///31-1 0
  ///rrrr F
  ///F - focused
  uint8_t* load; //init function
  uint8_t* update; //update function
  uint8_t* draw; //draw function
  uint8_t* event; //event handler function
} ep_window;

void eelphant_main(size_t width, size_t height);
ep_window* eelphant_create_window();
void eelphant_destroy_window(ep_window* w);

#endif
