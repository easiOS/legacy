#ifndef H_NET_UDP
#define H_NET_UDP

//User Datagram Protocol

#include <stdint.h>

typedef struct {
	uint16_t src, dst;
	uint16_t length;
	uint16_t checksum;
	uint8_t data[0];
} udp_header;

udp_header* udp_create(udp_header* header, uint16_t src, uint16_t dst, void* data, uint16_t len);
unsigned short internet_checksum(udp_header* header, void* addr, int count);

#endif