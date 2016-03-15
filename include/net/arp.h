#ifndef H_ARP
#define H_ARP

#define ARP_CACHE_SIZ 128
#define ARP_CACHE_EXPT 600000

#define ARP_HW_ETHERNET 0x0100

#define ARP_PROT_IPV4 0x0008

#define ARP_OP_REQ 0x0100
#define ARP_OP_REP 0x0200

struct arp_packet {
	uint16_t htype, ptype; //hardware type, protocol type
	uint8_t hwalen, protalen; //hw address len, prot address len
	uint16_t op; //operation
	uint16_t __attribute__((packed)) sha[3];
	uint16_t __attribute__((packed)) spa[2];
	uint16_t __attribute__((packed)) tha[3];
	uint16_t __attribute__((packed)) tpa[2];
} __attribute__((packed));

void arp_receive(struct ethernet_device* dev, void* data);
const uint8_t* arp_send_request(struct ethernet_device* dev, uint8_t* target_p); //target_p: target's protocol address,
//if target's address is cached then return pointer to that, otherwise a NULL
void arp_send_reply(struct ethernet_device* dev, uint16_t* sender_h, uint16_t* sender_p, uint16_t* target_h, uint16_t* target_p);

uint8_t* arp_block_until_known(uint8_t* target); //blocks until 100ms passed or target host's hw addr is known,
//returns with pointer to 0xFFFFFFFFFFFF or hw addr, with respect to either case
//note: this DOES NOT send a request

struct arp_entry {
	uint8_t protocol[4];
	uint16_t htype;
	uint8_t hwaddr[6];
	uint32_t flags;
	uint64_t expires; //the time when this entry will expire. if this is less than ticks(), then arp_send_request shall return with NULL
};

#endif