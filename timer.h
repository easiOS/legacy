#ifndef H_TIMER
#define H_TIMER

void init_timer(uint32_t frequency);
uint32_t ticks();
uint32_t millis();
void sleep(uint32_t millis);
void read_rtc();
uint32_t* get_time();
#endif