#ifndef H_STDIO
#define H_STDIO
#include <stdint.h>
#include <stddef.h>
#include "video.h"
#include <stdarg.h>

//File
typedef uint32_t FILE;

FILE *fopen( const char *filename, const char *mode );
FILE *fopen( const char *restrict filename, const char *restrict mode );
int32_t fclose( FILE *stream );
size_t fread( void *restrict buffer, size_t size, size_t count, FILE *restrict stream );
size_t fwrite( const void *restrict buffer, size_t size, size_t count, FILE *restrict stream );

//std I/O
int32_t puts( const char *str );
int32_t putchar( int32_t ch );
int32_t getchar_nb(); //non-blocking
int32_t getchar();
int32_t scanf( const char *format, ... );
int32_t printf( const char *format, ... );

char *gets( char *str ); //deprecated

#endif
