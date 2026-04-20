#include "terminal.hpp"
#include "io.hpp"

// Constructor: Initializes the terminal state
Terminal::Terminal() {
    row = 0;
    col = 0;
    color = 0x0F; // White on Black
    buffer = (uint16_t*) 0xB8000;
    clear();
}

void Terminal::clear() {
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        buffer[i] = make_vga_entry(' ', color);
    }
}

void Terminal::updateCursor() {
  uint16_t pos = row * VGA_WIDTH + col;

    // Tell the VGA board we are setting the low byte (0x0F)
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));

    // Tell the VGA board we are setting the high byte (0x0E)
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void Terminal::put_char(char c) {
    if (c == '\n') {
        col = 0;
        row++;
    } else {
        const size_t index = row * VGA_WIDTH + col;
        buffer[index] = make_vga_entry(c, color);
        
        if (++col == VGA_WIDTH) {
            col = 0;
            row++;
        }
    }

    if (row >= VGA_HEIGHT) {
        scroll();
        row = VGA_HEIGHT - 1;
    }
    updateCursor();
}

void Terminal::write(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {
        put_char(data[i]);
    }
    updateCursor();
}

uint16_t Terminal::make_vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

void Terminal::scroll() {
    for (size_t i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); ++i) {
        buffer[i] = buffer[i + VGA_WIDTH];
    }
    for (size_t i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; ++i) {
        buffer[i] = make_vga_entry(' ', color);
    }
}