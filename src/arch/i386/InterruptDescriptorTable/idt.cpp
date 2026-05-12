#include "idt.hpp"

extern "C" {
    void isr0();
    void isr1();
    void isr2();
    void isr3();
    void isr4();
    void isr5();
    void isr6();
    void isr7();
    void isr8();
    void isr9();
    void isr10();
    void isr11();
    void isr12();
    void isr13();
    void isr14();
    void isr15();
    void isr16();
    void isr17();
    void isr18();
    void isr19();
    void isr20();
    void isr21();
    void isr22();
    void isr23();
    void isr24();
    void isr25();
    void isr26();
    void isr27();
    void isr28();
    void isr29();
    void isr30();
    void isr31();
    void isr32();
    void isr33();

    void idt_load(uint32_t);
}

// Array of ISR handlers
static void (*const interrupt_handlers[])() = {
    isr0,  isr1,  isr2,  isr3,
    isr4,  isr5,  isr6,  isr7,
    isr8,  isr9,  isr10, isr11,
    isr12, isr13, isr14, isr15,
    isr16, isr17, isr18, isr19,
    isr20, isr21, isr22, isr23,
    isr24, isr25, isr26, isr27,
    isr28, isr29, isr30, isr31,
    isr32, isr33
};

IDT::IDT() {}

void IDT::load() {
    // Clear IDT
    for (int i = 0; i < 256; i++) {
        set_gate(i, 0, 0, 0);
    }

    pointer.limit = sizeof(idt_entry) * 256 - 1;
    pointer.base  = (uint32_t)&entries;

    for (uint8_t i = 0; i < sizeof(interrupt_handlers) / sizeof(interrupt_handlers[0]); i++) {
        set_gate(i,
                 (uint32_t)interrupt_handlers[i],
                 0x08,
                 0x8E);
    }

    idt_load((uint32_t)&pointer);
}

void IDT::set_gate(uint8_t num,
                   uint32_t base,
                   uint16_t sel,
                   uint8_t flags) {

    entries[num].base_low  = base & 0xFFFF;
    entries[num].base_high = (base >> 16) & 0xFFFF;
    entries[num].sel       = sel;
    entries[num].always0   = 0;
    entries[num].flags     = flags;
}

IDT interruptDescriptorTable;