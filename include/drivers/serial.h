#ifndef H_SERIAL
#define H_SERIAL

#define COM1 0x3f8
#define COM2 0x2f8
#define COM3 0x3e8
#define COM4 0x2e8

void sinit(void);
int sinitport(unsigned short port);
int senabled(unsigned short port);

int sempty(unsigned short port);
int savailable(unsigned short port);

char sread(unsigned short port);
void swrite(unsigned short port, char c);
void swrites(unsigned short port, const char* str);


#endif /* H_SERIAL */