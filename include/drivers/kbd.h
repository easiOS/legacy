#ifndef H_KBD
#define H_KBD

#define IOFLAGS_SHIFT 4
#define IOFLAGS_CTRL 5
#define IOFLAGS_ALT 6
#define IOFLAGS_GUI 7
#define IOFLAGS_DS 8

void kbdinit();
int kbdavailable();
struct input_event* kbdpoll();
void kbdsetlayout(char* layout, char* layout_shift);
void kbdresetlayout(void);

#endif