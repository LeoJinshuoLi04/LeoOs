#include "terminal.hpp"

extern Terminal* global_terminal; // Global pointer


struct registers {
    uint32_t ds;                  // Data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha
    uint32_t int_no, err_code;    // Interrupt number and error code
    uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically
};

// src/isr.cpp
#include "io.hpp"
#include "terminal.hpp"
#include "keyboard.hpp"

extern Terminal* global_terminal;

extern "C" void isr_handler(registers regs) {
    if (regs.int_no == 33) {
        uint8_t scancode = inb(0x60);

        // If the top bit is set, the key was just released
        if (scancode & 0x80) {
            // Handle key release if necessary (e.g., releasing Shift)
        } else {
            // Key was pressed
            unsigned char c = kbd_us[scancode];
            if (c > 0 && global_terminal) {
                global_terminal->put_char(c);
            }
        }
    } else if (regs.int_no == 32) {
      //do nothing.
    }

    // Send EOI to PICs
    if (regs.int_no >= 40) outb(0xA0, 0x20);
    if (regs.int_no >= 32) outb(0x20, 0x20);
}