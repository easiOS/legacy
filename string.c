#include "string.h"

char *strchr(const char *s, int c) {
    while (*s != (char) c) {
        if (!*s++) {
            return NULL;
        }
    }
    return (char *)s;
}

char *strpbrk(const char *s1, const char *s2)
{
    while(*s1)
        if(strchr(s2, *s1++))
            return (char*)--s1;
    return 0;
}

char *strsep(char **stringp, const char *delim) {
  char *begin, *end;

  begin = *stringp;
  if(begin == NULL)
    return NULL;

  if(delim[0] == '\0' || delim[1] == '\0') {
    char ch = delim[0];

    if(ch == '\0')
      end = NULL;
    else {
      if(*begin == ch)
        end = begin;
      else if(*begin == '\0')
        end = NULL;
      else
        end = strchr(begin + 1, ch);
    }
  }
  else
    end = strpbrk(begin, delim);

  if(end) {
    *end++ = '\0';
    *stringp = end;
  }
  else
    *stringp = NULL;

  return begin;
}

char* sp = NULL; /* the start position of the string */
 
char* strtok(char* str, const char* delimiters) {
 
    int i = 0;
    int len = strlen(delimiters);
 
    /* if the original string has nothing left */
    if(!str && !sp)
        return NULL;
 
    /* initialize the sp during the first call */
    if(str && !sp)
        sp = str;
 
    /* find the start of the substring, skip delimiters */
    char* p_start = sp;
    while(true) {
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