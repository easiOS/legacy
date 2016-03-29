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

char us_keys[] = {
  0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
  '-', '=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u',
  'i', 'o', 'p', '{', '}', '\n', 0, 'a', 's', 'd', 'f',
  'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '|', 'z', 'x',
  'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
  [74] = '-', [78] = '+', [127] = 0
};

char us_keys_shift[] = {
  0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
  '_', '+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U',
  'I', 'O', 'P', '[', ']', '\n', 0, 'A', 'S', 'D', 'F',
  'G', 'H', 'J', 'K', 'L', ':', '\"', '~', 0, '\'', 'Z', 'X',
  'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ',
  [74] = '-', [78] = '+', [127] = 0
};

#endif