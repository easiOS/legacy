#include <stddef.h>
#include <port.h>
#include <dtables.h>
#include <drivers/timer.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>

unsigned tick = 0;
unsigned last_rtc_read = 3600000;
unsigned long long ttime = 0;
unsigned century = 0;
unsigned year = 1970;
unsigned month = 1;
unsigned day = 1;
unsigned hour = 0;
unsigned minute = 0;
unsigned second = 0;

unsigned long long get_unix_time()
{
  return ttime;
}

static void timer_callback(registers_t regs)
{
  tick++;
  if(tick % 1000 == 0)
  {
    ttime++;
    if(second + 1 >= 60)
    {
      read_rtc();
      second = 0;
      if(minute + 1 > 60)
      {
        minute = 0;
        if(hour + 1 > 24)
        {
          hour = 0;
          int day31 = 0;
          switch(month)
          {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
            day31 = 1;
            break;
          }
          if(day + 1 > (month == 2) ? ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) ? 29 : 28 : (day31) ? 31 : 30)
          {
            day = 1;
            if(month > 12)
            {
              month = 1;
              year++;
            }
            else
              month++;
          }
          else
            day++;
        }
        else
          hour++;
      }
      else
        minute++;

    }
    else
      second++;
  }
  outb(0x20, 0x20); //End of Interrupt
}

  unsigned* get_time(unsigned* out)
  {
    out[0] = year;
    out[1] = month;
    out[2] = day;
    out[3] = hour;
    out[4] = minute;
    out[5] = second;
    return out;
  }

  unsigned ticks()
  {
    return tick;
  }

  unsigned millis()
  {
    return tick;
  }

  void timerinit(unsigned frequency)
  {
    puts("Timer register, frequency: ");
    char buffer[32];
    itoa(frequency, buffer, 10);
    puts(buffer);
    puts(" Hz...");
    // Firstly, register our timer callback.
    dtables_reg(IRQ0, &timer_callback);

    // The value we send to the PIT is the value to divide it's input clock
    // (1193180 Hz) by, to get our required frequency. Important to note is
    // that the divisor must be small enough to fit into 16-bits.
    unsigned divisor = 1193180 / frequency;
    // Send the command byte.
    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);
    puts("done.\n");
  }

  void __sleep(unsigned millis)
  {
    unsigned start = ticks();
    unsigned now = ticks();
    while(now - start < millis)
    {
      now = ticks();
      cpu_relax();
    }
  }

  void sleep(unsigned millis)
  {
    __sleep(millis);
  }
///RTC (CMOS)
//Request Port: 0x70
//Read Port: 0x71
//Register  Contents
// 0x00      Seconds
// 0x02      Minutes
// 0x04      Hours
// 0x06      Weekday
// 0x07      Day of Month
// 0x08      Month
// 0x09      Year
// 0x32      Century (maybe)
// 0x0A      Status Register A
// 0x0B      Status Register B

  int get_update_in_progress_flag() {
    outb(0x70, 0x0A);
    return (inb(0x71) & 0x80);
  }

  unsigned char get_RTC_register(int reg) {
    outb(0x70, reg);
    return inb(0x71);
  }

  void read_rtc() {
      /*if(ticks() - last_rtc_read < 30000)
        return;*/
    last_rtc_read = ticks();
    unsigned char century;
    unsigned char last_second;
    unsigned char last_minute;
    unsigned char last_hour;
    unsigned char last_day;
    unsigned char last_month;
    unsigned char last_year;
    unsigned char last_century;
    unsigned char registerB;

      // Note: This uses the "read registers until you get the same values twice in a row" technique
      //       to avoid getting dodgy/inconsistent values due to RTC updates

      while (get_update_in_progress_flag());                // Make sure an update isn't in progress
      second = get_RTC_register(0x00);
      minute = get_RTC_register(0x02);
      hour = get_RTC_register(0x04);
      day = get_RTC_register(0x07);
      month = get_RTC_register(0x08);
      year = get_RTC_register(0x09);
      unsigned char century_register = 0x32;
      if(century_register != 0) {
        century = get_RTC_register(century_register);
      }

      do {
        last_second = second;
        last_minute = minute;
        last_hour = hour;
        last_day = day;
        last_month = month;
        last_year = year;
        last_century = century;

            while (get_update_in_progress_flag());           // Make sure an update isn't in progress
            second = get_RTC_register(0x00);
            minute = get_RTC_register(0x02);
            hour = get_RTC_register(0x04);
            day = get_RTC_register(0x07);
            month = get_RTC_register(0x08);
            year = get_RTC_register(0x09);
            if(century_register != 0) {
              century = get_RTC_register(century_register);
            }
          } while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
           (last_day != day) || (last_month != month) || (last_year != year) ||
           (last_century != century) );

          registerB = get_RTC_register(0x0B);

      // Convert BCD to binary values if necessary

          if (!(registerB & 0x04)) {
            second = (second & 0x0F) + ((second / 16) * 10);
            minute = (minute & 0x0F) + ((minute / 16) * 10);
            hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
            day = (day & 0x0F) + ((day / 16) * 10);
            month = (month & 0x0F) + ((month / 16) * 10);
            year = (year & 0x0F) + ((year / 16) * 10);
            if(century_register != 0) {
              century = (century & 0x0F) + ((century / 16) * 10);
            }
          }

      // Convert 12 hour clock to 24 hour clock if necessary

          if (!(registerB & 0x02) && (hour & 0x80)) {
            hour = ((hour & 0x7F) + 12) % 24;
          }

      // Calculate the full (4-digit) year

          if(century_register != 0) {
            year += century * 100;
          } else {
            year += (1970 / 100) * 100;
            if(year < 1970) year += 100;
          }
      //puts("RTC: OK\n");
      //seconds_since_1970 = tm_sec + tm_min*60 + tm_hour*3600 + (last_month * )*86400 + (last_year-70)*31536000 + ((tm_year-69)/4)*86400 - ((tm_year-1)/100)*86400 + ((tm_year+299)/400)*86400;
        }

void time_init(void)
{
  read_rtc();
  timerinit(1000);
  ttime = 31536000 * (year - 1970);
  int days = 0;
  for(int i = 1970; i < year - 1970; i++)
  {
    if(i % 4 != 0)
      continue;
    if(i % 100 != 0)
    {
      days++;
      break;
    }
    if(i % 400 != 0)
      continue;
    days++;
  }
  for(int i = 1; i < month; i++)
  {
    switch(i)
    {
      case 2:
      days += 28;
      break;
      case 4:
      case 6:
      case 9:
      case 11:
      days += 30;
      break;
      default:
      days += 31;
      break;
    }
  }
  ttime += days * 86400 + hour * 3600 + minute * 60 + second;
}