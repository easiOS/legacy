#ifndef H_SERIAL
#define H_SERIAL

void init_serial();
char read_serial();
int serial_received();
int is_transmit_empty();
void write_serial(char a);
int serial_enabled();

#endif
