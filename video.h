#ifndef H_VIDEO
#define H_VIDEO

#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>

enum vga_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

uint8_t make_color(enum vga_color fg, enum vga_color bg);
uint16_t make_vgaentry(char c, uint8_t color);
size_t strlen(const char* str);
void terminal_initialize();
void terminal_setcolor(uint8_t color);
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y);
void terminal_putchar(char c);
void terminal_writestring(const char* data);
void terminal_writestringat(const char* data, size_t x, size_t y);
void terminal_test();
void terminal_writeint(uint32_t n);
void terminal_setcursor(uint16_t x, uint16_t y);
void terminal_clear();
void terminal_prfx(char* prefix, char* str);
void terminal_prfxi(uint32_t prefix, char* str);
uint32_t terminal_getx();
uint32_t terminal_gety();
void terminal_fill(char c, size_t x, size_t y, size_t w, size_t h);
void concat(char p[], char q[]);
char terminal_getcharat(size_t x, size_t y);
#endif