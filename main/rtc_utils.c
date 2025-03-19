#include <stdio.h>
#include "rtc_utils.h"
#include "hardware/rtc.h"

void rtc_init_config(uint year, uint month, uint day, uint hour, uint min, uint sec) {
    datetime_t rtc_time = {
        .year  = year,
        .month = month,
        .day   = day,
        .hour  = hour,
        .min   = min,
        .sec   = sec,
    };
    rtc_init();
    rtc_set_datetime(&rtc_time);
}

void rtc_get_formatted_time(char *buffer, size_t buffer_size) {
    datetime_t now;
    rtc_get_datetime(&now);
    snprintf(buffer, buffer_size, "%02d:%02d:%02d", now.hour, now.min, now.sec);
}
