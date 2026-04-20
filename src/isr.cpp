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

extern Terminal* global_terminal;

extern "C" void isr_handler(registers regs) {
    if (regs.int_no == 33) {
        uint8_t scancode = inb(0x60);
        
        // Just a test: print 'A' if the 'A' key (0x1E) is pressed
        if (scancode == 0x1E) {
            if (global_terminal) global_terminal->write("A");
        }

        // --- THE MOST IMPORTANT PART ---
        // Send EOI to PIC1 (and PIC2 if it was IRQ 8-15)
        // Without this, no more interrupts will ever fire!
        if (regs.int_no >= 40) {
            outb(0xA1, 0x20); // Slave
        }
        outb(0x20, 0x20);     // Master
    } else if (regs.int_no == 32) {
      //do nothing.
    }

    // Send EOI to PICs
    if (regs.int_no >= 40) outb(0xA0, 0x20);
    if (regs.int_no >= 32) outb(0x20, 0x20);
}