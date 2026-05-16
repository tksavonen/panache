// panache/drivers/time/cmos.c

#include <cmos.h>
#include <serial.h>

#define CMOS_ADDRESS 0x70
#define CMOS_DATA    0x71

#define BCD_TO_BIN(val) ((val & 0x0F) + ((val >> 4) * 10))

uint8_t cmos_read(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

void cmos_get_time(cmos_time_t *t) {
    // wait for CMOS update to finish
    while (cmos_read(0x0A) & 0x80);

    uint8_t seconds = cmos_read(0x00);
    uint8_t minutes = cmos_read(0x02);
    uint8_t hours   = cmos_read(0x04);
    uint8_t day     = cmos_read(0x07);
    uint8_t month   = cmos_read(0x08);
    uint8_t year    = cmos_read(0x09);

    uint8_t status_b = cmos_read(0x0B);
    if (!(status_b & 0x04)) {
        seconds = BCD_TO_BIN(seconds);
        minutes = BCD_TO_BIN(minutes);
        hours   = BCD_TO_BIN(hours);
        day     = BCD_TO_BIN(day);
        month   = BCD_TO_BIN(month);
        year    = BCD_TO_BIN(year);
    }

    t->seconds = seconds;
    t->minutes = minutes;
    t->hours   = hours;
    t->day     = day;
    t->month   = month;
    t->year    = 2000 + year;  // let's assume 21st century
}