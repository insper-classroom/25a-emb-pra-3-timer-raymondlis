#ifndef RTC_UTILS_H
#define RTC_UTILS_H

#include "pico/util/datetime.h"

void rtc_init_config(uint year, uint month, uint day, uint hour, uint min, uint sec);

void rtc_get_formatted_time(char *buffer, size_t buffer_size);

#endif
