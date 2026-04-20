#pragma once
#include <stdint.h>

struct idt_entry {
    uint16_t base_low;  // Lower 16 bits of the handler function address
    uint16_t sel;       // Our Code Segment selector (0x08)
    uint8_t  always0;   // Always set to 0
    uint8_t  flags;     // Type and attributes
    uint16_t base_high; // Upper 16 bits of the handler address
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

class IDT {
public:
    IDT();
    void load();
    void set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
private:
    idt_entry entries[256];
    idt_ptr pointer;
};