// panache/include/cmos.h

#ifndef CMOS_H
#define CMOS_H

#include <stdint.h>

typedef struct {
    uint8_t  seconds;
    uint8_t  minutes;
    uint8_t  hours;
    uint8_t  day;
    uint8_t  month;
    uint16_t year;
} cmos_time_t;

uint8_t cmos_read(uint8_t reg);

void cmos_get_time(cmos_time_t *t);

#endif