#include <stdint.h>
#include <stddef.h>
#include "string.h"

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
