#ifndef H_STDLIB
#define H_STDLIB

#include <stdint.h>
#include <stddef.h>

char* itoa(int64_t n, char* s, int base);
char* uitoa(uint64_t n, char* s, int base);
void* free(void* ptr);
void* malloc(size_t size);
int atoi(char* p);
unsigned atou(char* c);

#endif
