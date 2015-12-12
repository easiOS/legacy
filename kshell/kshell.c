#include <kshell.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dev/kbd.h>

#define KSHELL_PROMPT "easios>"
#define KSHELL_BUFSIZE 128

char shell_buffer[KSHELL_BUFSIZE];
int shell_buffer_i = 0;

void kshell_eval()
{
  memset(shell_buffer, 0, KSHELL_BUFSIZE);
  shell_buffer_i = 0;
}

void kshell_main()
{
  puts("kshell\n");
  struct keyevent* e = kbdpoll();
  while(1)
  {
    if(e)
    {
      switch(e->character)
      {
        case '\b':
        {
          if(!e->release)
          {
            shell_buffer[shell_buffer_i] = ' ';
            shell_buffer_i--;
          }
          break;
        }
        case '\n':
        {
          if(e->release)
          {
            kshell_eval();
          }
          break;
        }
        case '\t':
          break;
        default:
        {
          shell_buffer[shell_buffer_i] = e->character;
          break;
        }
      }
      if(shell_buffer_i > KSHELL_BUFSIZE - 1)
        shell_buffer_i = KSHELL_BUFSIZE - 1;

    }
  }
}
