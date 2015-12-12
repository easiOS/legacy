#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

char* strcat(char* destination, const char* source) {
   int c, d;
   c = 0;
   while (destination[c] != '\0') {
      c++;
   }
   d = 0;
   while (source[d] != '\0') {
      destination[c] = source[d];
      d++;
      c++;
   }
   destination[c] = '\0';
   return destination;
}

size_t strlen(const char* str)
{
	size_t ret = 0;
	while ( str[ret] != 0 )
		ret++;
	return ret;
}

void* memset(void *dest, int val, size_t len)
{
  unsigned char* tmp=dest;
    while(len--)
        *tmp++ = (unsigned char)val;
    return dest;
}

int strcmp(const char* str1, const char* str2)
{
    while(*str1 && (*str1==*str2))
        str1++,str2++;
    return *(const unsigned char*)str1-*(const unsigned char*)str2;
}

int strcoll(const char* str1, const char* str2)
{
  return strcmp(str1, str2);
}

char* strcpy(char* destination, const char* source)
{
  int i = 0;
  while ((destination[i] = source[i]) != '\0')
    i++;
  return destination;
}

/*char* strcpy(char* destination, const char* source)
{
  char* src = (char*)source;
  char* dst = destination;
  while(src != '\0')
  {
    *dst++ = *src++;
  }
  return destination;
}*/

void* memcpy(void* dest, const void* src, size_t count) {
  char* dst8 = (char*)dest;
  char* src8 = (char*)src;

  while (count--) {
    *dst8++ = *src8++;
  }
  return dest;
}

char* strtok(char* str, const char* delimiters)
{
    static int currIndex = 0;
    if(!str || !delimiters || str[currIndex] == '\0')
      return NULL;
    char *W = (char *)malloc(sizeof(char)*100);
    int i = currIndex, k = 0, j = 0;
    while (str[i] != '\0')
    {
      j = 0;
      while (delimiters[j] != '\0')
      {
        if (str[i] != delimiters[j])
          W[k] = str[i];
        else
          goto It;
        j++;
      }
      i++;
      k++;
    }
It:
    W[i] = 0;
    currIndex = i+1;
    return W;
}

size_t strspn (const char *str1, const char *str2)
{
  const char *p;
  const char *a;
  size_t count = 0;

  for (p = str1; *p != '\0'; ++p)
    {
      for (a = str2; *a != '\0'; ++a)
    if (*p == *a)
      break;
      if (*a == '\0')
    return count;
      else
    ++count;
    }

  return count;
}
