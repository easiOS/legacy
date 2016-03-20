#ifndef H_STDIO
#define H_STDIO

#include <stdint.h>

#define BUFSIZ 
#define EOF -1
#define FILENAME_MAX 256
#define FOPEN_MAX 64

#define FILE_FLAG_FREE (1 << 0)
#define FILE_SOCKET_TCP (1 << 31)
#define FILE_SOCKET_UDP (1 << 30)
#define FILE_SOCKET_RAW (1 << 29)

enum FILETYPE {
	FT_UNKNOWN = 0,
	FT_PHYSICAL = 1,
	FT_SYSTEM = 2,
	FT_DEVICE = 3,
	FT_SOCKET = 4
};

enum FILEMODE {
	FM_UNKNOWN = 0,
	FM_READ = 1,
	FM_WRITE = 2,
	FM_APPEND = 3,
	FM_READU = 4,
	FM_WRITEU = 5,
	FM_APPENDU = 6
};

typedef struct {
	unsigned id;
	unsigned flags;
	enum FILETYPE type;
	void* systemaddr;
	void* physicalfile;
	unsigned char ipv4[4];
	unsigned short port;
	int eof;
	enum FILEMODE mode;
} FILE;

int puts(const char* str);
int putc(char c);
char* gets(char* str);
int getchar();
int printf(const char* format, ...);
int snprintf(char* buffer, unsigned int buffer_len, const char *fmt, ...);

#endif
