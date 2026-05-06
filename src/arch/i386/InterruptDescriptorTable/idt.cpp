#include "idt.hpp"   
// Declare the assembly stub from interrupts.s
extern "C" void isr0(); 
extern "C" void isr1(); 
extern "C" void isr2(); 
extern "C" void isr3(); 
extern "C" void isr4(); 
extern "C" void isr5(); 
extern "C" void isr6(); 
extern "C" void isr7(); 
extern "C" void isr8(); 
extern "C" void isr9(); 
extern "C" void isr10(); 
extern "C" void isr11(); 
extern "C" void isr12(); 
extern "C" void isr13(); 
extern "C" void isr14(); 
extern "C" void isr15(); 
extern "C" void isr16(); 
extern "C" void isr17(); 
extern "C" void isr18(); 
extern "C" void isr19(); 
extern "C" void isr20(); 
extern "C" void isr21(); 
extern "C" void isr22(); 
extern "C" void isr23(); 
extern "C" void isr24(); 
extern "C" void isr25(); 
extern "C" void isr26(); 
extern "C" void isr27(); 
extern "C" void isr28(); 
extern "C" void isr29(); 
extern "C" void isr30(); 
extern "C" void isr31(); 
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
    set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
    set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
    set_gate(4, (uint32_t)isr4, 0x08, 0x8E);
    set_gate(5, (uint32_t)isr5, 0x08, 0x8E);
    set_gate(6, (uint32_t)isr6, 0x08, 0x8E);
    set_gate(7, (uint32_t)isr7, 0x08, 0x8E);
    set_gate(8, (uint32_t)isr8, 0x08, 0x8E);
    set_gate(9, (uint32_t)isr9, 0x08, 0x8E);
    set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
    set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
    set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
    set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
    set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
    set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
    set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
    set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
    set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
    set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
    set_gate(31, (uint32_t)isr31, 0x08, 0x8E);
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