#ifndef H_STDLIB
#define H_STDLIB

#include <stddef.h>

char* itoa(long long n, char* s, int base);
char* uitoa(long long n, char* s, int base);
void* free(void* ptr);
void* malloc(size_t size);
int atoi(char* p);

#endif
