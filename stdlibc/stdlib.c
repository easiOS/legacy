#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdio.h>

char* itoa(int64_t n, char* s, int base)
{
  char* orig = s;
  char tmp[33];
  memset(tmp, 0, 33);
  char *tp = tmp;
  int i;
  unsigned v;

  int sign = (base == 10 && n < 0);
  if (sign)
      v = -n;
  else
      v = (unsigned)n;

  while (v || tp == tmp)
  {
      i = v % base;
      v /= base;
      if (i < 10)
        *tp++ = i+'0';
      else
        *tp++ = i + 'a' - 10;
  }

  int len = tp - tmp;

  if (sign)
  {
      *s++ = '-';
      len++;
  }

  while (tp > tmp)
      *s++ = *--tp;
  *s = '\0';
  return orig;
}

void* malloc(size_t size)
{
  if(size <= 0){ puts("malloc: Size less than zero\n"); return 0;}
  unsigned address = (unsigned)kmalloc(size);
  return (void*)address;
}

void* free(void* ptr) //actually this is kfree but who cares lol
{
  if(!ptr) return NULL;
  kmfree(ptr);
  return ptr;
}

int isdigit(char c)
{
  return c >= 48 && c <= 57;
}

int atoi(char* c) {
    int value = 0;
    int sign = 1;
    if( *c == '+' || *c == '-' )
    {
        if( *c == '-' ) sign = -1;
        c++;
    }
    while (isdigit(*c))
    {
        value *= 10;
        value += (int) (*c-'0');
        c++;
    }
    return (value * sign);
}
