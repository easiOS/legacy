#ifndef H_TIMER
#define H_TIMER

void timerinit(uint32_t frequency);
uint32_t ticks();
uint32_t millis();
void sleep(uint32_t millis);
void read_rtc();
uint32_t* get_time(uint32_t* out);
uint64_t get_unix_time();
#endif
