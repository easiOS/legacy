//EasiOS Shell
#include "shell.h"

char sc_dict[] = {0, 27, '1', '2', '3', '4', '5', '6', '7',
	'8', '9', '0', '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y',
	'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's', 'd', 'f', 'g', 'h',
	'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b',
	'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


char scanc2char(uint32_t sc)
{
	return sc_dict[sc];
}

bool valid_char(uint32_t sc)
{
	if(!(sc & 0x80))
		return false;
	return sc_dict[sc] != 0;
}

bool shell_exit = false;
char shell_cmdbuf[128];
int shell_cmdbuf_c = 0;
user_t users[256];
user_t* current;

int strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1==*s2))
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

void uptime()
{
	uint32_t seconds = ticks() / 1000;
	uint32_t minutes = seconds / 60;
	uint32_t hours = minutes / 60;
	minutes -= hours * 60;
	seconds -= hours * 3600 + minutes * 60;
	terminal_writestring("Up since ");
	if(hours > 0)
	{
		terminal_writeint(hours); terminal_writestring(" hours, ");
	}
	if(minutes > 0 || hours > 0)
	{
		terminal_writeint(minutes); terminal_writestring(" minutes, ");
	}
	terminal_writeint(seconds); terminal_writestring(" seconds.\n");
}

void su()
{
	user_t* last = &users[current->id];
	current = &users[0];
	current->last = (void*)last;
}

void exit()
{
	if(((user_t*)current->last)->id == 1)
		shell_exit = true;
	else
		current = (user_t*)current->last;
}

void restart()
{
	if(current->superuser)
			reboot();
		else
			terminal_writestring("Only root can do that.\n");
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

void (*shell_function[64])(); //function table
char* shell_function_name[64];

void shell_process()
{
	char cmd[shell_cmdbuf_c+1];
	int l = 0;
	char c = 0;
	do
	{
		c = shell_cmdbuf[l++];
	}
	while(c != '\0');
	for(int i = 0; i < l; i++)
	{
		cmd[i] = shell_cmdbuf[i];
	}
	cmd[shell_cmdbuf_c+1] = '\0';
	bool ok = false;
	for(int i = 0; i < 64; i++)
	{
		if(strcmp(cmd, shell_function_name[i]) == 0)
		{
			(*shell_function[i])();
			ok = true;
		}
	}
	if(!ok)
	{
		terminal_writestring("Unknown command: ");
		terminal_writestring(cmd);
		terminal_writestring("\n");
	}
	for(int i = 0; i < 128; i++)
	{
		shell_cmdbuf[i] = '\0';
	}
	shell_cmdbuf_c = 0;
}

void shell_prompt()
{
	terminal_writestring("\n");
	terminal_writestring(current->name);
	char p[2];
	p[0] = current->prompt;
	terminal_writestring(p);
	terminal_writestring(" ");
	char c;
	while(c != '\n')
	{
		if(keyb_isavail())
		{
			//terminal_writestring("k");
			uint32_t held = keyb_get();
			//if(valid_char(held))
			{
				c = scanc2char(held);
				switch(c)
				{
					case '\n':
						terminal_writestring("\n");
						shell_process();
						break;
					case '\b':
						if(shell_cmdbuf_c > 1)
						{
							shell_cmdbuf[shell_cmdbuf_c] = ' ';
							shell_cmdbuf_c -= 1;
							terminal_setcursor(terminal_getx() - 1, terminal_gety());
						}
						break;
					case '\t': //autocomplete?
						break;
					case '\0':
						break;
					default:
						if(!(bool)(c & 0x80))
						{
							if(shell_cmdbuf_c < 128)
								shell_cmdbuf[shell_cmdbuf_c++] = c;
							char cs[2];
							cs[0] = c;
							terminal_writestring(cs);
							/*terminal_writeint(shell_cmdbuf_c);*/
						}
						break;
				}
			}
		}
	}
}

void shell_main()
{
	//set up users
	user_t root;
	root.name = "root";
	root.passw = "";
	root.prompt = '#';
	root.id = 0;
	root.superuser = true;
	users[0] = root;
	user_t seu; //shell exit user, special user where the top user exits
	seu.id = 1;
	users[1] = seu;
	user_t user;
	user.name = "user";
	user.passw = "";
	user.prompt = '$';
	user.id = 100;
	user.superuser = false;
	users[100] = user;
	users[100].last = &users[1];
	current = &users[100];
	//add functions to the table
	shell_function[0] = exit;
	shell_function_name[0] = "exit";
	shell_function[1] = su;
	shell_function_name[1] = "su";
	shell_function[2] = restart;
	shell_function_name[2] = "reboot";
	shell_function[3] = uptime;
	shell_function_name[3] = "uptime";
	shell_function[4] = calculator;
	shell_function_name[4] = "calculator";
	terminal_writestring("ESh 0.2\n");
	while(!shell_exit)
	{
		shell_prompt();
	}
}