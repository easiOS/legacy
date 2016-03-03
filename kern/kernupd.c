// EasiOS Kernel Updater

#include <dev/serial.h>
#include <dev/disk.h>
#include <fs/thinfat32.h>

const uint8_t kernupd_preamble[8] = {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xD5};

struct kernupd_packet {
	uint16_t signature; //0xabcd
	uint32_t id; //packet indentifier
	uint8_t op; //operation (0 - data, 1 - stx, 2 - etx, 3 - ack, 4 - retransmit, >5 - ignored, ff - debug)
	uint32_t reserved; //zero
	uint8_t checksum; //sum of all bytes of packet should be zero.
	uint8_t payload[500];
};

extern struct eos_drives drives[4];

char kernupd_readchar(void);

void kernupd_init(void)
{
	printf("\n==== ==== ==== ====\nInitializing kernel update\n");
	if(!serenabled(COM1))
	{
		printf("\tEnabling serial port\n");
		serinitport(COM1);
	}
	printf("\tenter kernupd_loop, RS\n");
	while(!sertxempty(COM1));
}

void kernupd_loop(void)
{
	uint8_t buffer[512];
	return;
}

void kernupd_retransm(int id)
{

}

int kernupd_recv(uint8_t* buffer)
{
	int pac = 0;
	while(pac < 8)
	{
		char c = kernupd_readchar();
		if(pac == 7 && c == 0xD5)
			break;
		else if(pac < 7 && c == 0x55)
			pac++;
		else
			pac = 0;
	}
	int i = 0;
	int sum = 0;
	while(i < 512)
	{
		uint8_t c = (uint8_t)kernupd_readchar();
		buffer[i++] = c;
		sum += c;
	}
	if(sum != 0)
		return 1;
	return 0;
}

void kernupd_txpkt(struct kernupd_packet* p)
{

}

char kernupd_readchar(void)
{
	while(!serrxavail(COM1));
	return serread(COM1);
}