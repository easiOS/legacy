#ifndef H_ITOA
#define H_ITOA
#include <stdint.h>
#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
void reverse(char str[], int length);
char* itoa(int num, char* str, int base);
long long int atoi(const char *c);
#endif