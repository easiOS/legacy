#ifndef H_SERIAL
#define H_SERIAL

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <port.h>

#define COM1 0x3f8
#define COM2 0x2f8
#define COM3 0x3e8
#define COM4 0x2e8

void serinit();
void serinitport(uint16_t port);
bool serenabled(uint16_t port);
bool serrxavail(uint16_t port);
char serread(uint16_t port);
bool sertxempty(uint16_t port);
void serwrite(uint16_t port, char a);
void serswrite(uint16_t port, const char* str);
void sersetmode(uint16_t port, uint8_t mode);
#endif
