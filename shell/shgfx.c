#include "shgfx.h"

void draw_border(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t color)
{
	//draw corners
	terminal_putentryat('#', color, x, y);
	terminal_putentryat('#', color, x+w, y);
	terminal_putentryat('#', color, x, y+h);
	terminal_putentryat('#', color, x+w, y+h);
	//top
	for(int i = x + 1; i < x+w; i++)
	{
		terminal_putentryat('#', color, i, y);
	}
	//bottom
	for(int i = x + 1; i < x+w; i++)
	{
		terminal_putentryat('#', color, i, y+h);
	}
	//left
	for(int i = y + 1; i < y+h; i++)
	{
		terminal_putentryat('#', color, x, i);
	}
	//right
	for(int i = y + 1; i < y+h; i++)
	{
		terminal_putentryat('#', color, x+w, i);
	}
}