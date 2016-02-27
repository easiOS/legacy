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

char* sp = NULL;

char* strtok(char* str, const char* delimiters) {

    int i = 0;
    int len = strlen(delimiters);

    /* check in the delimiters */
    if(len == 0){}

    /* if the original string has nothing left */
    if(!str && !sp)
        return NULL;

    /* initialize the sp during the first call */
    if(str && !sp)
        sp = str;

    /* find the start of the substring, skip delimiters */
    char* p_start = sp;
    while(1) {
        for(i = 0; i < len; i ++) {
            if(*p_start == delimiters[i]) {
                p_start ++;
                break;
            }
        }

        if(i == len) {
               sp = p_start;
               break;
        }
    }

    /* return NULL if nothing left */
    if(*sp == '\0') {
        sp = NULL;
        return sp;
    }

    /* find the end of the substring, and
        replace the delimiter with null */
    while(*sp != '\0') {
        for(i = 0; i < len; i ++) {
            if(*sp == delimiters[i]) {
                *sp = '\0';
                break;
            }
        }

        sp ++;
        if (i < len)
            break;
    }

    return p_start;
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

int memcmp(const void* ptr1, const void* ptr2, size_t num)
{
  const unsigned char *p1 = ptr1, *p2 = ptr2;
  while(num--)
  {
    if(*p1 != *p2)
      return *p1 - *p2;
    else
      p1++,p2++;
  }
  return 0;
}

int strncmp(const char *_l, const char *_r, size_t n)
{
	const unsigned char *l=(void *)_l, *r=(void *)_r;
	if (!n--) return 0;
	for (; *l && *r && n && *l == *r ; l++, r++, n--);
	return *l - *r;
}

char *strrchr(char *s, int c)
{
  char* ret = 0;
  do
  {
    if(*s == c)
      ret = (char*)s;
  } while(*s++);
  return (ret);
}

char* strchr(char* str, int character)
{
  int l = strlen(str);
  for(int i = 0; i < l; i++)
  {
    if(str[i] == character)
      return str + i;
  }
  return NULL;
}

char* strncpy(char *dst, const char* src, size_t n)
{
char *temp = dst;
while (n-- && (*dst++ = *src++));
return temp;
}

char tolower(char c)
{
  return (c < 91 && c >= 65 ? c + 32 : c > 97 ? c - 32 : c);
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
  const char* ps1 = s1;
  const char* ps2 = s2;
  int cmpval;
  if(!ps1) return 0;
  if(!ps2) return 0;
  if(n != 0)
  {
    do
    {
      cmpval = tolower(*ps1) - tolower(*ps2);
      if(cmpval != 0)
      {
        return cmpval;
      }
      ++ps1;
      ++ps2;
      if(*ps1 == 0) break;
    } while(--n != 0);
  }
  return 0;
}