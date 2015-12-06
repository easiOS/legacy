#ifndef H_MOUSE
#define H_MOUSE

#include <stdint.h>
#include <stdbool.h>

struct mouseevent {
  int dx, dy;
  bool left, right, middle;
} mouseevents[256];

void mouseinit();

bool mouseavail();
struct mouseevent* mousepoll();

#endif
