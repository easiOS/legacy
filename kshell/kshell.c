#include <kshell.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dev/kbd.h>
#include <kernel.h>
#include <video.h>
#include <dev/graphics.h>
#include <eelphant.h>

#define KSHELL_PROMPT "easios>"
#define KSHELL_BUFSIZE 128

char shell_buffer[KSHELL_BUFSIZE];
int shell_buffer_i = 0;

void kshell_eval()
{
  /*if(shell_buffer_i) return;
  if(shell_buffer[0] == '\0') return;*/
  char* args[8];
  memset(args, 0, 32);
  int argc = 0;
  int cmdlen = -1;
  char copy[128];
  memcpy(copy, shell_buffer, 128);
  for(int i = 0; copy[i] != '\0'; cmdlen = i++);
  cmdlen++;
  char* token = strtok(copy, " ");
  while(token)
  {
    args[argc++] = token;
    token = strtok(NULL, " ");
  }
  #define CMDCMP(str) if(strcmp(args[0], str) == 0)
  CMDCMP("resolution")
  {
    if(argc < 2) return;
    int w = 0, h = 0;
    w = atoi(args[1]);
    h = atoi(args[2]);
    if(w == 0 || h == 0) return;
    if(graphics_available())
    {
      puts("setting res...\n");
      //vbgfx_set_res(w, h);
      graphics_set_mode(w, h, 32);
    }
    else
    {
      puts("resolution: no graphics card available\n");
    }
    return;
  }
  CMDCMP("reboot")
  {
    reboot("Requested by user");
    return;
  }
  CMDCMP("eelphant")
  {
    size_t w = vgetw();
    size_t h = vgeth();
    int ret;
    do
    {
      ret = eelphant_main(w, h);
    } while (ret);
    return;
  }
  puts("\nUnknown command: "); puts(args[0]); putc('\n');
}

void kshell_main()
{
  puts("kshell\n");
  struct keyevent* e = kbdpoll();
  puts(KSHELL_PROMPT);
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
            if(shell_buffer_i < 0) shell_buffer_i = 0;
            putc('\b');
          }
          break;
        }
        case '\n':
        {
          if(e->release)
          {
            shell_buffer[shell_buffer_i] = '\0';
            kshell_eval();
            memset(shell_buffer, 0, KSHELL_BUFSIZE);
            shell_buffer_i = 0;
            puts("\n"KSHELL_PROMPT);
          }
          break;
        }
        case '\t':
          break;
        default:
        {
          if(!e->release)
          {
            shell_buffer[shell_buffer_i++] = e->character;
            putc(e->character);
          }
          break;
        }
      }
      if(shell_buffer_i > KSHELL_BUFSIZE - 1)
        shell_buffer_i = KSHELL_BUFSIZE - 1;
    }
    e = kbdpoll();
  }
}
