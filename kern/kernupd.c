// EasiOS Kernel Updater

#include <string.h>
#include <dev/serial.h>
#include <dev/disk.h>
#include <fs/thinfat32.h>
#include <kernel.h>
#include <video.h>

const char END = 0xD5; //start/end of file
const char ESC = 0xA5; //escape

extern struct eos_drives drives[4];

TFFile* kernupdf = NULL;

char kernupd_readchar(void);
void kernupd_sendchar(char c);
void kernupd_init(void)
{
	printf("\n==== ==== ==== ====\nInitializing kernel update\n");
	for(int i = 0; i < 4; i++)
	{
		if(drives[i].type != 2 && i == 3)
		{
			printf("No FAT32 partition found. Can't update!\n");
			return;
		}
		else if(drives[i].type == 2)
			break;
		else
			continue;
	}
	if(!serenabled(COM1))
	{
		printf("\tEnabling serial port\n");
		serinitport(COM1);
	}
	printf("\tenter loop\n");
	kernupdf = tf_fopen((uint8_t*)"/sys/kernel", (const uint8_t*)"w");
	uint8_t c;
	int phase = 0;
	while(phase < 2)
	{
		c = (uint8_t)kernupd_readchar();
		if(phase == 0 && c != 0xD5) continue;
		switch(c)
		{
			case 0xD5:
			{
				phase = 2;
				vsetcol(255, 0, 0, 255);
				vd_rectangle(FILL, 0, 0, 100, 100);
				break;
			}
			case 0xA5:
				c = (uint8_t)kernupd_readchar();
			default:
			{
				tf_fwrite(&c, 1, 1, kernupdf);
				tf_fflush(kernupdf);
				break;
			}
		}
		printf("%x ", c);
	}
	tf_fclose(kernupdf);
	printf("\nUpdate complete!\n");
}

char kernupd_readchar(void)
{
	return serread(COM1);
}

void kernupd_sendchar(char c)
{
	while(!sertxempty(COM1));
	serwrite(COM1, c);
}

void kernupd_finish(void)
{
	printf("kernupd_finish\n");
	if(!kernupdf) return;
	tf_fclose(kernupdf);
	reboot("Kernel update finished");
}