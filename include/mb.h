// panache/include/mb.h

#ifndef MB_H
#define MB_H

#include <stdint.h>

struct mb2_info {
    uint32_t total_size;
    uint32_t reserved;
};

struct mb2_tag {
    uint32_t type;
    uint32_t size;
};

struct mb2_tag_framebuffer {
    uint32_t type;
    uint32_t size;
    uint64_t addr;
    uint32_t pitch;
    uint32_t width;
    uint32_t height;
    uint8_t  bpp;
    uint8_t  type_fb;
    uint8_t  reserved[2];
};

#endif