#ifndef H_NET
#define H_NET

#include <config.h>

#define htons(A) ((((unsigned short)(A) & 0xff00) >> 8) | \
(((unsigned short)(A) & 0x00ff) << 8))
#define htonl(A) ((((unsigned)(A) & 0xff000000) >> 24) | \
(((unsigned)(A) & 0x00ff0000) >> 8) | \
(((unsigned)(A) & 0x0000ff00) << 8) | \
(((unsigned)(A) & 0x000000ff) << 24))
#define ntohs htons
#define ntohl htohl

typedef struct netif netif;

struct netif
{
	char name[16];
	
	unsigned flags;
	
	unsigned char hwaddr[6];
	struct {
		unsigned char addr[4];
		unsigned char mask[4];
		unsigned char gway[4];
	} inet_addr;
	struct {
		unsigned short addr[8];
		unsigned char mask;
		unsigned short gway[8];
	} inet6_addr;

	int link;
	unsigned received, sent;

	void (*read)(netif*, void*, int); //receive int frames to void*
	void (*write)(netif*, void*, int); //send int frames from void*

	void* private;
};

netif* netif_allocate(void);
void netif_free(netif* i);
void netif_list(void);
const netif* netif_getbyindex(int index);

#endif