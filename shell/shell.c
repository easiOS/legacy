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

bool valid_char(uint8_t sc)
{
	if(!(sc & 0x80))
		return false;
	if(sc > sizeof(sc_dict) / sizeof(char)) return false;
	return sc_dict[sc] != 0;
}

bool shell_exit = false;
char shell_cmdbuf[128];
char shell_lastcmd[128];
int shell_cmdbuf_c = 0;
user_t users[256];
user_t* current;
size_t cursor_default_x = 0;

void shell_request_exit()
{
	shell_exit = true;
}

bool shell_login()
{
	char uname[128];
	char pass[128];
	memset(uname, 0, 128);
	memset(pass, 0, 128);
	terminal_writestring("Login: ");
	int cc = 0;
	char c = 0;
	do
	{
		if(keyb_isavail())
		{
			c = scanc2char(keyb_get());
			switch(c)
			{
				case '\n':
					terminal_writestring("\n");
					break;
				case '\b':
					if(cc > 0)
					{
						uname[cc] = '\0';
						cc -= 1;
						//terminal_setcursor(terminal_getx() - 1, terminal_gety());
						//terminal_putentryat(' ', COLOR_LIGHT_GREY | COLOR_BLACK << 4, terminal_getx(), terminal_gety());
					}
					break;
				case '\t': //autocomplete?
					break;
				case '\0':
					break;
				default:
					if(!(bool)(c & 0x80))
					{
						if(cc < 128)
							uname[cc++] = c;
						//char cs[2];
						//cs[0] = c;
						//terminal_writestring(cs); //login is hidden for the exxtra securityez
					}
					break;
			}
		}
	}
	while(c != '\n');
	terminal_writestring("Password: ");
	cc = 0;
	c = 0;
	do
	{
		if(keyb_isavail())
		{
			c = scanc2char(keyb_get());
			switch(c)
			{
				case '\n':
					terminal_writestring("\n");
					break;
				case '\b':
					if(cc > 0)
					{
						pass[cc] = '\0';
						cc -= 1;
						//terminal_setcursor(terminal_getx() - 1, terminal_gety());
						//terminal_putentryat(' ', COLOR_LIGHT_GREY | COLOR_BLACK << 4, terminal_getx(), terminal_gety());
					}
					break;
				case '\t': //autocomplete?
					break;
				case '\0':
					break;
				default:
					if(!(bool)(c & 0x80))
					{
						if(cc < 128)
							pass[cc++] = c;
						//char cs[2];
						//cs[0] = c;
						//terminal_writestring(cs);
						/*terminal_writeint(shell_cmdbuf_c);*/
					}
					break;
			}
		}
	}
	while(c != '\n');
	 bool unameok = true;
	 bool passok = true;
	for(int i = 0; i < 256; i++)
	{
		for(int i = 0; i < 128; i++)
		{
			if(uname[i] == '\0' && users[i].name[i] != '\0')
				unameok = false;
			else if(uname[i] != '\0' && users[i].name[i] == '\0')
				unameok = false;
			else if(uname[i] == users[i].name[i])
				unameok = true;
			else
				unameok = false;
			if(!unameok)
				break;
		}
		for(int i = 0; i < 128; i++)
		{
			if(pass[i] == '\0' && users[i].passw[i] != '\0')
				passok = false;
			else if(pass[i] != '\0' && users[i].passw[i] == '\0')
				passok = false;
			else if(pass[i] == users[i].passw[i])
				passok = true;
			else
				unameok = false;
			if(!passok)
				break;
		}
		if(unameok && passok)
		{
			break;
		}
	}
	return unameok && passok;
}

bool shell_auth(char* uname)
{
	terminal_writestring("Password for ");
	terminal_writestring(uname);
	terminal_writestring(": ");
	char pass[128];
	int cc = 0;
	char c = 0;
	do
	{
		if(keyb_isavail())
		{
			c = scanc2char(keyb_get());
			switch(c)
			{
				case '\n':
					terminal_writestring("\n");
					break;
				case '\b':
					if(cc > 0)
					{
						pass[cc] = '\0';
						cc -= 1;
						//terminal_setcursor(terminal_getx() - 1, terminal_gety());
						//terminal_putentryat(' ', COLOR_LIGHT_GREY | COLOR_BLACK << 4, terminal_getx(), terminal_gety());
					}
					break;
				case '\t': //autocomplete?
					break;
				case '\0':
					break;
				default:
					if(!(bool)(c & 0x80))
					{
						if(cc < 128)
							pass[cc++] = c;
						//char cs[2];
						//cs[0] = c;
						//terminal_writestring(cs);
						/*terminal_writeint(shell_cmdbuf_c);*/
					}
					break;
			}
		}
	}
	while(c != '\n');
	for(int i = 0; i < 256; i++)
	{
		if(strcmp(uname, users[i].name) == 0 && strcmp(pass, users[i].passw) == 0)
		{
			current = &users[i];
			return true;
		}
	}
	return false;
}

void su()
{
	if(shell_auth("root"))
	{
		if(current == &users[0])
			return;
		user_t* last = &users[current->id];
		current = &users[0];
		current->last = (void*)last;
		cowsay_c("I ", 2);
	}
	else
	{
		terminal_writestring("su: failed\n");
	}
}

void exit()
{
	if(((user_t*)current->last)->id == /*(user_t*)*/current->id)
		shell_request_exit();
	else
		current = (user_t*)current->last;
}

int strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1==*s2))
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
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
		shell_lastcmd[i] = cmd[i];
	}
	cmd[shell_cmdbuf_c+1] = '\0';
	shell_lastcmd[shell_cmdbuf_c + 1] = '\0';
	bool ok = false;
	if(strcmp(cmd, "help") == 0)
	{
		terminal_writestring("Commands: \n");
		for(int i = 0; i < 64; i++)
		{
			if(shell_function_name[i] != NULL)
			{
				terminal_writestring(shell_function_name[i]);
				terminal_writestring("\n");
			}
		}
		memset(shell_cmdbuf, '\0', 128);
		shell_cmdbuf_c = 0;
		return;
	}
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
	memset(shell_cmdbuf, '\0', 128);
	shell_cmdbuf_c = 0;
}

void shell_prompt()
{
	terminal_writestring("\n");
	terminal_writestring(current->name);
	char p[2];
	p[0] = current->prompt;
	p[1] = '\0';
	terminal_writestring(p);
	terminal_writestring(" ");
	char c;
	while(c != '\n')
	{
		if(keyb_isavail())
		{
			uint32_t held = keyb_get();
			if(held == 0x48)
			{
				terminal_setcursor(cursor_default_x, terminal_gety());
				for(int i = 0; i < 128; i++)
				{
					shell_cmdbuf[i] = shell_lastcmd[i];
					terminal_putchar(shell_cmdbuf[i]);
				}
				memset(shell_lastcmd, 0, 128);

				continue;
			}
			//if(valid_char(held))
			{
				c = scanc2char(held);
				switch(c)
				{
					case '\n':
						terminal_writestring("\n");
						cursor_default_x = terminal_getx();
						shell_process();
						break;
					case '\b':
						if(shell_cmdbuf_c > 0)
						{
							shell_cmdbuf[shell_cmdbuf_c] = '\0';
							shell_cmdbuf_c -= 1;
							terminal_setcursor(terminal_getx() - 1, terminal_gety());
							terminal_putentryat(' ', COLOR_LIGHT_GREY | COLOR_BLACK << 4, terminal_getx(), terminal_gety());
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

user_t* shell_get_current_user()
{
	return current;
}

void shell_main()
{
	memset(shell_lastcmd, 0, 128);
	memset(shell_cmdbuf, 0, 128);
	//set up users
	user_t root;
	root.name = "root";
	root.passw = "root";
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
	shell_function[5] = breakout;
	shell_function_name[5] = "breakout";
	shell_function[6] = date;
	shell_function_name[6] = "date";
	shell_function[7] = click;
	shell_function_name[7] = "click";
	shell_function[8] = panic;
	shell_function_name[8] = "panic";
	shell_function[9] = cowsay;
	shell_function_name[9] = "cowsay";
	shell_function[10] = clear;
	shell_function_name[10] = "clear";
	shell_function[11] = cowsay_fortune;
	shell_function_name[11] = "fortune";
	terminal_writestring("ESh 0.2\n");
	//bool login_success = false;
	//while(!login_success)
	//{
	//	login_success = shell_login();
	//}
	while(!shell_exit)
	{
		shell_prompt();
	}
}
