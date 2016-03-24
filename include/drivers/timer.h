#ifndef H_TIMER
#define H_TIMER

void time_init(void);
void timerinit(unsigned frequency);
unsigned ticks();
unsigned millis();
void sleep(unsigned millis);
void read_rtc();
unsigned* get_time(unsigned* out);
unsigned long long get_unix_time();
#endif
