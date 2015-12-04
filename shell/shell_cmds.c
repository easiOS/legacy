#include "shell_cmds.h"
#include "../video.h"
#include "../keyboard.h"
#include "../kernel.h"
#include "../timer.h"
#include "../mouse.h"
#include "shell.h"
#include "../itoa.h"
#include <string.h>

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
				case '0':
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
	uint32_t game_mx = 48;
	uint32_t game_my = 20;
	uint32_t fgc = 0;
	terminal_clear();
	draw_border(game_ox - 1, game_oy - 1, game_mx, game_my, COLOR_LIGHT_GREY | COLOR_BLACK << 4);
	//terminal_writestringat("BREAKOUT", 12, 3);
	terminal_writestringat("Press Q to quit", 51, 7);
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
		return true;
	}

	ball_t game_ball;
	game_ball.x = 10;
	game_ball.y = 10;
	game_ball.t = 0;
	bool game_exit = false;
	uint16_t framerate_lock = 1000/60; //silky smooth 60 fps
	//uint8_t color = COLOR_WHITE | COLOR_BLACK << 4;
	draw_border(game_ox - 1, game_oy - 1, game_mx, game_my, COLOR_LIGHT_GREY | COLOR_BLACK << 4);
	while(!game_exit)
	{
		sleep(framerate_lock);
		if(keyb_isavail())
		{
			game_exit = (scanc2char(keyb_get()) == 'q');
		}
		//terminal_fill(' ', 2, 2, 31, 15);
		//draw_border(1, 1, 32, 16, COLOR_LIGHT_GREY | COLOR_BLACK << 4);
		bounce(&game_ball);
		terminal_putentryat('O', fgc | COLOR_BLACK << 4, game_ball.x, game_ball.y);
		if(fgc == 15)
		{
			fgc = 0;
		}
		else
			fgc++;
	}
	terminal_clear();
}

void click()
{
	if(was_click())
	{
		terminal_writestring("Click!\n");
	}
	else
	{
		terminal_writestring("No click :(\n");
	}
}

void panic()
{
	switch_to_user_mode();
}

void cowsay_c(const char* input, uint32_t cc)
{
	terminal_writestring("\n");
	//uint32_t color = COLOR_LIGHT_GREY | COLOR_BLACK << 4;
	for(int i = 0; i < cc; i++)
	{
		if(i == 78)
		{
			terminal_putchar('\n');
		}
		terminal_putchar(input[i]);
	}
	terminal_putchar('\n');
	terminal_setcursor(0, terminal_gety() + 4);
	terminal_writestring("        \\   ^__^\n");
	terminal_writestring("         \\  (oo)\\_______\n");
	terminal_writestring("            (__)\\       )\\/\\\n");
	terminal_writestring("                ||----w |\n");
	terminal_writestring("                ||     ||\n");
}

void cowsay()
{
	/*
 _____________
< hello human >
 -------------
        \   ^__^
         \  (oo)\_______
            (__)\       )\/\
                ||----w |
                ||     ||
	*/
	char input[79];
	char c = 0;
	uint32_t cc = 0;
	do
	{
		if(keyb_isavail())
		{
			c = scanc2char(keyb_get());
			if(c == '\b')
			{
				if(cc > 0)
				{
					input[cc] = '\0';
					cc -= 1;
					terminal_setcursor(terminal_getx() - 1, terminal_gety());
					terminal_putentryat(' ', COLOR_LIGHT_GREY | COLOR_BLACK << 4, terminal_getx(), terminal_gety());
				}
			}
			else if(cc < 79 && c != '\n' && c != '\0' && c != '\b')
			{
				input[cc++] = c;
				char asd[2];
				asd[0] = c;
				asd[1] = '\0';
				terminal_writestring(asd);
			}
		}
	} while(c != '\n');
	cowsay_c(input, cc);
	memset(input, 0, 79);
	cc = 0;
	c = 0;
}

static uint32_t next = -1;

void cowsay_fortune()
{
	if(next == -1)
	{
		next = ticks();
	}
	next = next * 1103515245 + ticks();
	int random = ((next/65536) % 32768) % 11;
	char* wisdom[32];
	wisdom[0] = "Real Men don't make backups. They upload it via ftp and let the world mirror it.\n\t--Linus Torvalds";
	wisdom[1] = "If a 'train station' is where a train stops, what's a 'workstation'?";
	wisdom[2] = "A feature is nothing more than a bug with seniority.";
	wisdom[3] = "You can not get anything worthwhile done without raising a sweat.\n-- The First Law Of Thermodynamics\nWhat ever you want is going to cost a little more than it is worth.\n-- The Second Law Of Thermodynamics\nYou can not win the game, and you are not allowed to stop playing.\n-- The Third Law Of Thermodynamics";
	wisdom[4] = "Dopeler effect: the tendency of stupid ideas to seem smarter when they come at you rapidly.";
	wisdom[5] = "The sum of the Universe is zero.";
	wisdom[6] = "This quote intentionally left blank.";
	wisdom[7] = "Q: Why was Stonehenge abandoned?\nA: It wasn't IBM compatible.";
	wisdom[8] = "Q: How many IBM CPU's does it take to do a logical right shift?\nA: 33. 1 to hold the bits and 32 to push the register.";
	wisdom[9] = "I've heard a Jew and a Muslim argue in a Damascus cafe with less passion than the emacs wars.";
	wisdom[10] = "\"We all know Linux is great...it does infinite loops in 5 seconds.\"";
	wisdom[11] = "In most countries selling harmful things like drugs is punishable.\nThen howcome people can sell Microsoft software and go unpunished?";
	char c = 0;
	int i = 0;
	do
	{
		c = wisdom[random][i++];
	} while(c != '\0');
	cowsay_c(wisdom[random], i);
}

void clear()
{
	terminal_clear();
}
