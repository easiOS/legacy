#ifndef H_ITOA
#define H_ITOA
#include <stdint.h>
#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
void reverse(char s[]);
void itoa(int64_t n, char s[]);
void uitoa(uint64_t n, char s[]);
long long int atoi(const char *c);
#endif
