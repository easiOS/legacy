#include <stdio.h>
#include <stdint.h>
#include <video.h>
#include <amethyst.h>
#include <string.h>
#include <stdlib.h>
#include <fs/fat32/fat_filelib.h>

struct bmp_header
{
	char type[2];
	unsigned int size;
	unsigned int res1;
	unsigned int off;
	unsigned int hsiz;
	int width;
	int height;
	unsigned short planes;
	unsigned short bpp;
	unsigned int comp;
	unsigned int sizeimg;
	int xppm, yppm;
	unsigned int colused, colimp;
} __attribute__((packed));

struct image_windata
{
	enum {
		TUNK, // unknown
		T256, // 256-color
		T24 // 24-bit
	} type;
	rgb_t* palette;
	unsigned char* bitmap;
} __attribute__((packed));

uint32_t image_bswap32(uint32_t x)
{
	return	((x << 24) & 0xff000000 ) |
		((x <<  8) & 0x00ff0000 ) |
		((x >>  8) & 0x0000ff00 ) |
		((x >> 24) & 0x000000ff );
}

int image_unload(am_win* w)
{
	struct image_windata* wd = w->windata;
	free(wd->palette);
	free(wd->bitmap);
	free(w->windata);
	return 0;
}

int image_load(am_win* w)
{
	strcpy(w->title, "easiOS Photo Viewer");
	w->x = 25;
	w->y = 50;
	w->bg.r = 212;
	w->bg.g = 212;
	w->bg.b = 212;
	w->bg.a = 255;
	return 0;
}

void image_update(am_win* w, unsigned dt)
{

}

void image_draw_t24(am_win* w, int bx, int by)
{
	if(!w->windata)
		return;
	struct image_windata* d = w->windata;
	unsigned char* ci = d->bitmap;
	for(int y = 0; y < w->h; y++)
	{
		for(int x = 0; x < w->w; x++)
		{
			vsetcol(ci[2], ci[1], ci[0], 255);
			vplot(bx + x, by + w->h - y);
			ci+=3;
		}
		ci += (w->w % 4);
	}
}

void image_draw_t256(am_win* w, int bx, int by)
{
	if(!w->windata)
		return;
	struct image_windata* d = w->windata;

	for(int y = 0; y < w->h; y++)
	{
		for(int x = 0; x < w->w; x++)
		{
			rgb_t c;
			unsigned index = d->bitmap[(y * w->w) + x];
			c = d->palette[index];
			vsetcol(c.b, c.g, c.r, 255);
			vplot(bx + x, by + w->h - y);
		}
	}
}

void image_event(am_win* w, am_event* e)
{
	
}

int image_loadbmp256(am_win* w, struct bmp_header* h, FL_FILE* f);
int image_loadbmp24(am_win* w, struct bmp_header* h, FL_FILE* f);

int image_main(int argc, char** argv)
{
	if(argc < 2)
	{
		printf("image: argument needed (filename)\n");
		return 1;
	}
	char fnbuf[256];
	fnbuf[0] = '\0';
	strcat(fnbuf, "/user/");
	strcat(fnbuf, argv[1]);
	FL_FILE* f = fl_fopen(fnbuf, "r");
	if(!f)
	{
		printf("image: file /user/%s does not exist\n", argv[1]);
		return 1;
	}
	am_win* w = amethyst_create_window();
	if(!w)
	{
		printf("image: can't allocate window\n");
		fl_fclose(f);
		return 1;
	}
	w->load = &image_load;
	w->unload = &image_unload;
	w->update = &image_update;
	w->event = &image_event;
	w->load(w);
	amethyst_set_active(w);

	struct bmp_header h;
	fl_fread(&h, sizeof(struct bmp_header), 1, f);
	if(strncmp(h.type, "BM", 2) != 0)
	{
		printf("image: File not 256-bit BMP\n");
		goto destroy;
	}
	w->windata = malloc(sizeof(struct image_windata));
	if(!w->windata)
	{
		printf("image: cannot allocate memory to load image\n");
		goto destroy;
	}
	printf("image: reported size: %dx%d\n", h.width, h.height);
	printf("image: offset: %d\n", h.off);
	int ret = 0;
	if(h.bpp == 8)
		ret = image_loadbmp256(w, &h, f);
	if(h.bpp == 24)
		ret = image_loadbmp24(w, &h, f);
	if(ret)
	{
		free(w->windata);
		goto destroy;
	}
	return 0;

	destroy:
	fl_fclose(f);
	amethyst_destroy_window(w);
	return 1;
}

int image_loadbmp24(am_win* w, struct bmp_header* h, FL_FILE* f)
{
	w->draw = &image_draw_t24;
	w->w = h->width; w->h = h->height;
	struct image_windata* wd = w->windata;
	wd->bitmap = malloc(((h->bpp * h->width + 31) / 32) * 4 * h->height);
	if(!wd->bitmap)
	{
		printf("image: cannot allocate memory to load image\n");
		return 1;
	}
	unsigned siz = ((h->bpp * h->width + 31) / 32) * 4 * h->height;
	memset(wd->bitmap, 0xff, siz);
	/*uint8_t* bm_end = (uint8_t*)wd->bitmap + h->sizeimg;
	int rowc = 1;
	int rowsz = ((h->bpp * h->width + 31) / 32) * 4;
	for(uint8_t* bm = (uint8_t*)wd->bitmap; bm < bm_end; bm++)
	{
		if(rowc == h->width)
		{
			char buf[rowsz - (h->width * 3)];
			fl_fread(buf, rowsz - (h->width * 3), 1, f);
			rowc = 1;
		}
		rowc++;
		fl_fread(bm, 3, 1, f);
	}*/
	fl_fseek(f, h->off, SEEK_SET);
	fl_fread(wd->bitmap, siz, 1, f);
	fl_fclose(f);
	return 0;
}

int image_loadbmp256(am_win* w, struct bmp_header* h, FL_FILE* f)
{
	w->draw = &image_draw_t256;
	int pn = h->colused;
	if(pn == 0)
		pn = 256;
	w->w = h->width; w->h = h->height;
	struct image_windata* wd = w->windata;
	wd->type = T256;
	wd->palette = malloc(pn * 4);
	if(!wd->palette)
	{
		printf("image: cannot allocate memory to load image\n");
		return 1;
	}
	wd->bitmap = malloc(h->sizeimg);
	if(!wd->bitmap)
	{
		printf("image: cannot allocate memory to load image\n");
		free(wd->palette);
		return 1;
	}
	memset(wd->bitmap, 0xff, h->sizeimg);
	fl_fread(wd->palette, pn * 4, 1, f);
	fl_fread(wd->bitmap, h->sizeimg, 1, f);
	fl_fclose(f);
	printf("image: bitmap buffer addr: 0x%x", wd->bitmap);
	return 0;
}