#ifndef H_KBD
#define H_KBD

#include <stddef.h>
#include <stdbool.h>

struct keyevent {
  int keycode;
  char character;
  bool release;
  bool shift;
  bool ctrl;
  bool alt;
  bool doublescan;
} keyevents[256];

void kbdinit();
bool kbdavail();
struct keyevent* kbdpoll();

void kbdregsig(bool* b);
void kbdunregsig(bool* b);

#endif
