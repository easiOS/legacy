#include <stdlib.h>
#include <string.h>

char* itoa(int64_t n, char* s, int base)
{
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
  return s;
}
