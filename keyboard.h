#ifndef H_KEYB
#define H_KEYB
#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
void keyb_init();
//bool* keyb_get();
bool keyb_isavail();
uint32_t keyb_get();
char keyb_s2c(uint32_t scancode);
void keyb_clr();
void keyb_pak(); //press any key
bool ps2test();
#endif