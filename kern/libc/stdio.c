#include <stdio.h>
#include <dev/serial.h>
#include <text.h>

int puts(const char* str)
{
  if(tisinit())
  {
    tswrite(str);
  }
  if(serenabled(COM1))
  {
    serswrite(COM1, str);
  }
  int c = 0;
  while(str[c++] != '\0');
  return c;
}

int putc(char c)
{
  if(tisinit())
  {
    tputc(c, tgetcolor());
  }
  if(serenabled(COM1))
  {
    serwrite(COM1, c);
  }
  return 1;
}
