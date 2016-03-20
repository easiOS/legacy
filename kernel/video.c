#include <stdio.h>
#include <stddef.h>
#include <video.h>

unsigned* fb; //Framebuffer
unsigned __attribute__((aligned(4)))fbb[1440000]; //maximum resolution: 1600x900 or equivalent
long long fbw = 0, fbh = 0, fbbpp = 0, fbp = 0;
unsigned char fbt;
rgb_t color;

int vinit(long long width, long long height, long long bpp, long long pitch, unsigned int addr)
{
	fb = (unsigned*)addr;
	fbw = width;
	fbh = height;
	fbbpp = bpp;
	fbp = pitch;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	color.a = 255;
	for(long long y = 0; y < fbh; y++)
	{
		for(long long x = 0; x < fbw; x++)
		{
			fb[y * width + x] = 0;
			fbb[y * width + x] = 0;
		}
	}
	printf("video: Framebuffer @ 0x%x Backbuffer @ 0x%x\n", fb, fbb);
	return 0;
}

void vdestroy(void)
{
	fb = NULL;
	fbw = 0; fbh = 0; fbbpp = 0; fbp = 0;
	color.r = 0; color.g = 0; color.b = 0; color.a = 0;
}

void vswap(void)
{
	unsigned int* fb32 = (unsigned int*)fb;
	unsigned int* fbb32 = (unsigned int*)fbb;
	for(long long i = 0; i < fbw * fbh; i++)
		fb32[i] = fbb32[i];
}