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

bool strcmp(char str1[], char str2[])
{
	uint32_t sizeof1 = sizeof(str1) / sizeof(char);
	uint32_t sizeof2 = sizeof(str2) / sizeof(char);
	if(sizeof1 != sizeof2)
		return false;
	for(int i = 0; i < sizeof1; i++)
	{
		if(str1[i] != str2[i])
			return false;
	}
	return true;
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
	terminal_writestring("command: ");
	terminal_writestring(cmd);
	terminal_writestring("\n");
	if(strcmp(cmd, "uptime"))
	{
		uptime();
	}
	else if(strcmp(cmd, "su"))
	{
		su();
	}
	else if(strcmp(cmd, "exit"))
	{
		exit();
	}
	else
	{
		terminal_writestring(cmd); terminal_writestring(":");
		terminal_writestring(" unknown command\n");
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
	terminal_writestring("ESh 0.2\n");
	while(!shell_exit)
	{
		shell_prompt();
	}
}