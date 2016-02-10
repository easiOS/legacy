#include <net/udp.h>

unsigned short internet_checksum(void* addr, int count)
{
    /* Compute Internet Checksum for "count" bytes
     *         beginning at location "addr".
     */
    register long sum = 0;

    while( count > 1 )  {
    	/*  This is the inner loop */
        sum += * (unsigned short *) addr++;
        count -= 2;
    }

    /*  Add left-over byte, if any */
    if( count > 0 )
        sum += * (unsigned char *) addr;

    /*  Fold 32-bit sum to 16 bits */
    while (sum>>16)
        sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}

udp_header* udp_create(udp_header* header, uint16_t src, uint16_t dst, void* data, uint16_t len)
{
	header->src = src;
	header->dst = dst;
	header->length = len;
	header->checksum = internet_checksum(data, len);
	return header;
}