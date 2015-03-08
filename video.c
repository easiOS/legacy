#include "video.h"
#include "port.h"

uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

uint16_t make_vgaentry(char c, uint8_t color)
{
	uint16_t c16 = c;
	uint16_t color16 = color;
	return c16 | color16 << 8;
}

size_t strlen(const char* str)
{
	size_t ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;


static void terminal_move_cursor()
{
    // The screen is 80 characters wide...
    uint16_t cursorLocation = terminal_row * 80 + terminal_column;
    outb(0x3D4, 14);                  // Tell the VGA board we are setting the high cursor byte.
    outb(0x3D5, cursorLocation >> 8); // Send the high cursor byte.
    outb(0x3D4, 15);                  // Tell the VGA board we are setting the low cursor byte.
    outb(0x3D5, cursorLocation);      // Send the low cursor byte.
}

void terminal_clear()
{
	for ( size_t y = 0; y < VGA_HEIGHT; y++ )
	{
		for ( size_t x = 0; x < VGA_WIDTH; x++ )
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = make_vgaentry(' ', terminal_color);
		}
	}
	terminal_row = 0;
	terminal_column = 0;
	terminal_move_cursor();
}

void terminal_initialize()
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
	terminal_buffer = (uint16_t*) 0xB8000;
	terminal_clear();
	terminal_move_cursor();
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = make_vgaentry(c, color);
}

static void scroll()
{

   // Get a space character with the default colour attributes.
   uint8_t attributeByte = (COLOR_BLACK << 4) | (COLOR_LIGHT_GREY & 0x0F);
   uint16_t blank = 0x20 /* space */ | (attributeByte << 8);

   // Row 25 is the end, this means we need to scroll up
   if(terminal_row >= 25)
   {
       // Move the current text chunk that makes up the screen
       // back in the buffer by a line
       int i;
       for (i = 0*80; i < 24*80; i++)
       {
           terminal_buffer[i] = terminal_buffer[i+80];
       }

       // The last line should now be blank. Do this by writing
       // 80 spaces to it.
       for (i = 24*80; i < 25*80; i++)
       {
           terminal_buffer[i] = blank;
       }
       // The cursor should now be on the last line.
       terminal_row = 24;
   }
} 

void terminal_putchar(char c)
{
	if(c == '\n')
	{
		terminal_row++;
		terminal_column = 0;
	}
	else if(c == '\b')
	{
		terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
		terminal_row -= 1;

	}
	else if(c == '\t')
	{
		terminal_row += 4;
	}
	else
	{
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if ( ++terminal_column >= VGA_WIDTH )
		{
			terminal_column = 0;
			if ( ++terminal_row >= VGA_HEIGHT )
			{
				terminal_row = 0;
			}
		}
	}
	scroll();
	terminal_move_cursor();
}

void terminal_writestring(const char* data)
{
	size_t datalen = strlen(data);
	for ( size_t i = 0; i < datalen; i++ )
		terminal_putchar(data[i]);
}

void terminal_test()
{
	for(int i = 32; i < 128; i++)
	{
		terminal_putchar((char)i);
	}
}

void terminal_writeint(uint32_t n)
{

    if (n == 0)
    {
        terminal_putchar('0');
        return;
    }

    int32_t acc = n;
    char c[32];
    int i = 0;
    while (acc > 0)
    {
        c[i] = '0' + acc%10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    char c2[32];
    c2[i--] = 0;
    int j = 0;
    while(i >= 0)
    {
        c2[i--] = c[j++];
    }
    terminal_writestring(c2);

}

void terminal_setcursor(uint16_t x, uint16_t y)
{
	terminal_row = y;
	terminal_column = x;
	terminal_move_cursor();
}

void terminal_prfx(char* prefix, char* str)
{
	terminal_writestring("[");
	terminal_writestring(prefix);
	terminal_writestring("]");
	terminal_writestring(str);
}

void terminal_prfxi(uint32_t prefix, char* str)
{
	terminal_writestring("[");
	terminal_writeint(prefix);
	terminal_writestring("] ");
	terminal_writestring(str);
}

uint32_t terminal_getx()
{
	return terminal_column;
}

uint32_t terminal_gety()
{
	return terminal_row;
}

void concat(char p[], char q[]) {
   int c, d;
 
   c = 0;
 
   while (p[c] != '\0') {
      c++;  	
   }
 
   d = 0;
 
   while (q[d] != '\0') {
      p[c] = q[d];
      d++;
      c++;	
   }
 
   p[c] = '\0';
}