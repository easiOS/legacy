#include "stdio.h"

char sc_dict[] = {0, 27, '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y',
	'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h',
	'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b',
	'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

char sc_dict_shift[] = {0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
	'_', '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
	'\n', 0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '`',
	0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', 
	'?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

char sc_dict_ctrl[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int32_t puts( const char *str )
{
	int cc = 0;
	char c = 0;
	do
	{
		c = str[cc++];
		if(c != 0)
			terminal_putchar(str[c]);
	} while (c != 0);
	return cc;
}

int32_t putchar( int32_t ch )
{
	terminal_putchar(ch);
	return 0;
}

int32_t getchar()
{
	bool e = false;
	bool shift = false;
	bool ctrl = false;
	while(!e)
	{
		e = keyb_isavail();
		if(e)
		{
			int32_t sc = keyb_get();
			if(sc == 0x2A || sc == 0x36)
			{
				shift = true;
				e = false;
			}
			else if(sc == 0xAA || sc == 0xB6)
			{
				shift = false;
			}
			else if(sc == 0x1D)
			{
				ctrl = true;
				e = false;
			}
			else if(sc == 0x9D)
			{
				ctrl = false;
			}
			else if(sc >> 7 & 1 > 0)
			{
				e = false;
			}
			else
			{
				return (shift) ? sc_dict_shift[sc] : (ctrl) ? sc_dict_ctrl[sc] : sc_dict[sc];
			}
		}
	}
}

int32_t getchar_nb()
{
	bool e = false;
	bool shift = false;
	while(!e)
	{
		e = keyb_isavail();
		if(e)
		{
			int32_t sc = keyb_get();
			if(sc == 0x2A || sc == 0x36)
			{
				shift = true;
				e = false;
			}
			else if(sc == 0xAA || sc == 0xB6)
			{
				shift = false;
			}
			else if(sc >> 7 & 1 > 0)
			{
				e = false;
			}
			else
			{
				return (shift) ? sc_dict_shift[sc] : sc_dict[sc];
			}
		}
		else
		{
			return 0;
		}
	}
}

char *gets( char *str )
{
	char input[128];
	char c = 0;
	uint32_t cc = 0;
	bool n = false;
	do
	{
		char c = getchar();
		terminal_putchar(c);
		input[cc] = c;
		if(c == '\n')
			n = true;
		else if(c == '\b')
		{
			input[cc] = 0;
			cc--;
			terminal_setcursor(terminal_getx() - 1, terminal_gety());
		}
		else
			cc++;
	} while (!n);
	input[cc + 1] = '\0';
	str = input;
	return input;
}