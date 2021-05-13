#ifndef RTC_TIME_H
#define RTC_TIME_H

typedef enum {
      SECONDS = 0x00,
      MINUTES = 0x02,
      HOURS = 0x04,
      DAY = 0x07,
      MONTH = 0x08,
      YEAR = 0x09
} t_timeInfo;

#endif