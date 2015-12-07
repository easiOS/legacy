#ifndef H_SERIAL
#define H_SERIAL

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <port.h>

#define COM1 0x3f8

void serinit();
void serinitport(uint16_t port);
bool serenabled(uint16_t port);
bool serrxavail(uint16_t port);
char serread(uint16_t port);
bool sertxempty(uint16_t port);
void serwrite(uint16_t port, char a);
void serswrite(uint16_t port, const char* str);
#endif
