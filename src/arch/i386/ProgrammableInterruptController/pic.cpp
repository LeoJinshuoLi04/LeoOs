#include "src/arch/i386/ports.hpp"
#include "pic.hpp"

// PIC 1 (Master) ports
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
// PIC 2 (Slave) ports
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

void pic_remap() {
    // Save masks (what interrupts are currently disabled)
    // uint8_t a1 = inb(PIC1_DATA);
    // uint8_t a2 = inb(PIC2_DATA);

    // ICW1: Start initialization in cascade mode
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);

    // ICW2: Remap offset address
    // We tell Master PIC to start at 32 (0x20)
    // and Slave PIC to start at 40 (0x28)
    outb(PIC1_DATA, 0x20); 
    outb(PIC2_DATA, 0x28);

    // ICW3: Tell Master PIC there is a slave PIC at IRQ2 (0000 0100b)
    outb(PIC1_DATA, 4);
    // ICW3: Tell Slave PIC its cascade identity (0000 0010b)
    outb(PIC2_DATA, 2);

    // ICW4: Set 8086 mode
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    // Restore masks (or clear them to enable all IRQs)
    outb(PIC1_DATA, 0x0);
    outb(PIC2_DATA, 0x0);
}