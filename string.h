#ifndef H_STRING
#define H_STRING

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

char *strchr(const char *s, int c);
char *strpbrk(const char *s1, const char *s2);
char *strsep(char **stringp, const char *delim);
char* strtok(char* str, const char* delimiters);
#endif