#include "shell_cmds.h"
#include "../video.h"
#include "../keyboard.h"
#include "../kernel.h"
#include "../timer.h"

void uptime()
{
	uint32_t seconds = ticks() / 1000;
	uint32_t minutes = seconds / 60;
	uint32_t hours = minutes / 60;
	minutes -= hours * 60;
	seconds -= hours * 3600 + minutes * 60;
	terminal_writestring("Uptime: ");
	if(hours > 0)
	{
		terminal_writeint(hours); terminal_writestring(" hours, ");
	}
	if(minutes > 0 || hours > 0)
	{
		terminal_writeint(minutes); terminal_writestring(" minutes, ");
	}
	terminal_writeint(seconds); terminal_writestring(" seconds.\n");
	terminal_writestring("Boot date: ");
	uint32_t* t = get_boot_date();
	terminal_writeint(t[0]); terminal_writestring(". ");
	terminal_writeint(t[1]); terminal_writestring(". ");
	terminal_writeint(t[2]); terminal_writestring(". ");
	terminal_writeint(t[3]); terminal_writestring(":");
	terminal_writeint(t[4]); terminal_writestring(":");
	terminal_writeint(t[5]); terminal_writestring(" (UTC)\n");
}

void restart()
{
	user_t* current = shell_get_current_user();
	if(current->superuser)
			reboot();
		else
			terminal_writestring("Only root can do that.\n");
}

void date()
{
	terminal_writestring("Current date: ");
	uint32_t* t = get_time();
	terminal_writeint(t[0]); terminal_writestring(". ");
	terminal_writeint(t[1]); terminal_writestring(". ");
	terminal_writeint(t[2]); terminal_writestring(". ");
	terminal_writeint(t[3]); terminal_writestring(":");
	terminal_writeint(t[4]); terminal_writestring(":");
	terminal_writeint(t[5]); terminal_writestring(" (UTC)\n");
}

void calculator()
{
	terminal_clear();
	terminal_writestring("EasiOS Calculator\n");
	terminal_writestring("First number: ");
	char inp[8];
	char c;
	int cc = 0;
	do
	{
		if(keyb_isavail())
		{
			c = scanc2char(keyb_get());
			if(cc < 7 && c != '\n' && c != '\0')
			{
				inp[cc++] = c;
				char asd[2];
				asd[0] = c;
				terminal_writestring(asd);
			}
		}
	} while(c != '\n');
	int a = atoi(inp);
	for(int i = 0; i < 8; i++)
	{
		inp[i] = '\0';
	}
	c = 0;
	cc = 0;
	terminal_writestring(" Second number: ");
	do
	{
		if(keyb_isavail())
		{
			c = scanc2char(keyb_get());
			if(cc < 7 && c != '\n' && c != '\0')
			{
				inp[cc++] = c;
				char asd[2];
				asd[0] = c;
				terminal_writestring(asd);

			}
		}
	} while(c != '\n');
	int b = atoi(inp);
	c = 0;
	signed char op = -1;
	terminal_writestring("\nOperation (a - add, b - sub, c - mul, d - div): ");
	do
	{
		if(keyb_isavail())
		{
			c = scanc2char(keyb_get());
			terminal_writestring("\n");
			switch(c)
			{
				case 'a':
					terminal_writeint(a);
					terminal_writestring("+");
					terminal_writeint(b);
					terminal_writestring("=");
					terminal_writeint(a + b);
					terminal_writestring("\n");
					op = 0;
					break;
				case 'b':
					terminal_writeint(a);
					terminal_writestring("-");
					terminal_writeint(b);
					terminal_writestring("=");
					terminal_writeint(a - b);
					terminal_writestring("\n");
					op = 0;
					break;
				case 'c':
					terminal_writeint(a);
					terminal_writestring("*");
					terminal_writeint(b);
					terminal_writestring("=");
					terminal_writeint(a * b);
					terminal_writestring("\n");
					op = 0;
					break;
				case 'd':
					if(b == 0)
					{
						terminal_writestring("Can't divide by zero.\n");
						break;
					}
					terminal_writeint(a);
					terminal_writestring("/");
					terminal_writeint(b);
					terminal_writestring("=");
					terminal_writeint(a / b);
					terminal_writestring("\n");
					op = 0;
					break;
				default:
					break;
			}
		}
	} while(op == -1);

}

void breakout()
{
	typedef struct ball {
		uint32_t x,y,t;
	} ball_t;
	uint32_t game_ox = 2;
	uint32_t game_oy = 2;
	uint32_t game_mx = 32;
	uint32_t game_my = 16;
	terminal_clear();
	draw_border(1, 1, 32, 16, COLOR_LIGHT_GREY | COLOR_BLACK << 4);
	terminal_writestringat("BREAKOUT", 12, 3);
	terminal_writestringat("Press Q to quit", 40, 7);
	bool bounce(ball_t* b)
	{
		if(b->x <= game_ox && b->t == 1)
		{
			b->t = 0;
		}
		if(b->x >= game_mx && b->t == 0)
		{
			b->t = 1;
		}
		if(b->x <= game_ox && b->t == 2)
		{
			b->t = 3;
		}
		if(b->x >= game_mx && b->t == 3)
		{
			b->t = 2;
		}

		if(b->y <= game_oy && b->t == 1)
		{
			b->t = 2;
		}
		if(b->y >= game_my && b->t == 3)
		{
			b->t = 0;
		}
		if(b->y <= game_oy && b->t == 0)
		{
			b->t = 3;
		}
		if(b->y >= game_my && b->t == 2)
		{
			b->t = 1;
		}
		switch(b->t)
		{
			case 0:
				b->y--;
				b->x++;
				break;
			case 1:
				b->y--;
				b->x--;
				break;
			case 2:
				b->y++;
				b->x--;
				break;
			case 3:
				b->y++;
				b->x++;
				break;
		}
	}

	ball_t game_ball;
	game_ball.x = 10;
	game_ball.y = 10;
	game_ball.t = 0;
	bool game_exit = false;
	uint16_t framerate_lock = 1000/24; //silky smooth 24 fps
	uint8_t color = COLOR_WHITE | COLOR_BLACK << 4;
	draw_border(1, 1, 32, 16, COLOR_LIGHT_GREY | COLOR_BLACK << 4);
	while(!game_exit)
	{
		sleep(framerate_lock);
		if(keyb_isavail())
		{
			game_exit = (scanc2char(keyb_get()) == 'q');
		}
		terminal_fill(' ', 2, 2, 31, 15);
		//draw_border(1, 1, 32, 16, COLOR_LIGHT_GREY | COLOR_BLACK << 4);
		bounce(&game_ball);
		terminal_putentryat('O', color, game_ball.x, game_ball.y);
	}
	terminal_clear();
}