#ifndef H_STDIO
#define H_STDIO

int puts(const char* str);
int putc(char c);
char* gets(char* str);
int getchar();
int printf(const char* format, ...);
int snprintf(char* buffer, unsigned int buffer_len, const char *fmt, ...);

#endif
