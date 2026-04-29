#include "idt.hpp"   
// Declare the assembly stub from interrupts.s
extern "C" void isr0(); 
extern "C" void isr32();
extern "C" void isr33();
extern "C" void idt_load(uint32_t);

IDT::IDT() {
    // Zero out the entries
    for(int i = 0; i < 256; i++) {
        set_gate(i, 0, 0, 0);
    }

    // Set up the pointer
    pointer.limit = (sizeof(idt_entry) * 256) - 1;
    pointer.base  = (uint32_t)&entries;

    // Register our first exception: Division by Zero
    // 0x08 is our Kernel Code Segment, 0x8E is the flag for "Interrupt Gate"
    set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    set_gate(32, (uint32_t)isr32, 0x08, 0x8E);
    set_gate(33, (uint32_t)isr33, 0x08, 0x8E);
}

void IDT::load(){
  idt_load((int32_t)&pointer);
};

void IDT::set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags){
  entries[num].base_low = base & 0xFFFF;
  entries[num].base_high = base >> 16 & 0xFFFF;
  entries[num].sel = sel;
  entries[num].flags =  flags;
  entries[num].always0 =  0;
};