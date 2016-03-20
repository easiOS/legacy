#ifndef H_VIDEO
#define H_VIDEO

typedef struct {
	unsigned char r, g, b, a;
} rgb_t;

int vinit(long long width, long long height, long long bpp, long long pitch, unsigned int addr);
void vdestroy(void);
void vswap(void);

void vsetcolb(unsigned char r, unsigned char g, unsigned char b);
void vsetcols(rgb_t* color);
rgb_t* vgetcol(void);

void vcls(void);
void vplot(long long x, long long y);
void vplot_ub(long long x, long long y);

void vprint(const char* s, long long x, long long y, long long* xe, long long* ye);
void vprintl(const char* s, long long x, long long y, long long xl, long long yl);

long long vwidth(void);
long long vheight(void);

void vrect(int drawmode, long long x, long long y, long long w, long long h);
void vline(long long x1, long long y1, long long x2, long long y2);

void vbmp16(unsigned short* bmp, long long x, long long y, long long h);
void vbmp32(unsigned int* bmp, long long x, long long y, long long h);

void vcircle(long long x, long long y, long long radius);

#endif