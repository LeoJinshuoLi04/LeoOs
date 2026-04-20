#pragma once // Prevents double-including
#include <stddef.h>
#include <stdint.h>

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    // ... add more as you like
    VGA_COLOR_WHITE = 15,
};

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
#include "terminal.hpp"

class Terminal {
public:
    Terminal();
    void clear();
    void put_char(char c);
    void write(const char* data);
private:
    uint16_t make_vga_entry(unsigned char uc, uint8_t color);
    void scroll();
    size_t row, col;
    uint8_t color;
    uint16_t* buffer;
};