#ifndef H_NET_SLIP
#define H_NET_SLIP

//Serial Line Internet Protocol

#include <dev/ethernet.h>
#include <stdint.h>

/* SLIP special character codes
    */
#define END             0300    /* indicates end of packet */
#define ESC             0333    /* indicates byte stuffing */
#define ESC_END         0334    /* ESC ESC_END means END data byte */
#define ESC_ESC         0335    /* ESC ESC_ESC means ESC data byte */



//Initialize SLIP device on port
eth_dev_t* slip_dev_init(uint16_t port);
int slip_send_packet(void*, size_t, eth_dev_t*);
int slip_send_packet_nodev(void* p, size_t len, uint16_t port);

#endif