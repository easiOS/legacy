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

udp_header* udp_create(udp_header* header, uint16_t* srca, uint16_t* dsta, uint16_t src, uint16_t dst, void* data, uint16_t len);
void udp_send(void* srca, void* dsta, uint16_t srcp, uint16_t dstp, void* data, uint16_t len);
void udp_receive(void* srca, void* dsta, void* data, size_t len);

#endif