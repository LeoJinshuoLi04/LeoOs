#include "terminal.hpp"
#include "io.hpp"

// Constructor: Initializes the terminal state
Terminal::Terminal() {
    row = 0;
    col = 0;
    color = 0x0F; // White on Black
    buffer = (uint16_t*) 0xB8000;
    shell = nullptr;
    shellBuffer = "";
    clear();
}

Terminal::Terminal(Shell& shell) : shell(&shell){
    row = 0;
    col = 0;
    color = 0x0F; // White on Black
    buffer = (uint16_t*) 0xB8000;
    shellBuffer = "";
    clear();
}

void Terminal::clear() {
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        buffer[i] = make_vga_entry(' ', color);
    }
    shellBuffer = "";
    row = 0;
    col = 0;
    updateCursor();
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

void Terminal::put_user_char(char c){
    put_char(c);
    shellBuffer.push_back(c);
}

void Terminal::backspace(){
    if(shellBuffer.size() > 0){
        shellBuffer.pop_back();
        if(col == 0){
            if(row > 0){
                row --;
                col = VGA_WIDTH - 1;
            }
        }else{
            col--;
        }
        buffer[row* VGA_WIDTH + col] = make_vga_entry(' ', color);
    }
    updateCursor();
}

void Terminal::execute(){
    write("\n");
    shell->execute(shellBuffer);
    shellBuffer = "";
}

void Terminal::write_dec(int n) {
    if (n == 0) {
        write("0");
        return;
    }

    char buf[11]; // Max length of a 32-bit int + null
    int i = 10;
    buf[i] = '\0';

    while (n > 0) {
        buf[--i] = (n % 10) + '0';
        n /= 10;
    }
    write(&buf[i]);
}

void Terminal::write_hex(int n){
    string hex = "0x";
    string translations = "0123456789ABCDEF";
    while(n){
        hex.push_back(translations[n%16]);
        n/=16;
    }
    write(hex.cstr());
}

void Terminal::write(const char* data) {
    for (size_t i = 0; data[i] != '\0'; i++) {
        put_char(data[i]);
    }
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