#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <timer.h>

time_t time(time_t* timer)
{
  time_t t = get_unix_time();
  *timer = t;
  return t;
}
