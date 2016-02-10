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

#endif