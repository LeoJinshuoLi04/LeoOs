#include "gdt.hpp"

// External assembly function we defined in boot.s
extern "C" void gdt_flush(uint32_t);

GDT::GDT(){
  // 1. The Null Descriptor (All zeros)
    set_gate(0, 0, 0, 0, 0);

    /* 2. Kernel Code Segment
       Base: 0, Limit: 4GB, Access: 0x9A, Granularity: 0xCF
       0x9A = 10011010b (Present, Ring 0, Code, Executable, Readable)
    */
    set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* 3. Kernel Data Segment
       Base: 0, Limit: 4GB, Access: 0x92, Granularity: 0xCF
       0x92 = 10010010b (Present, Ring 0, Data, Writable)
    */
    set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    // Later we will add User Mode segments at index 3 and 4!

    // Prepare the pointer for the LGDT instruction
    pointer.limit = (sizeof(gdt_entry) * 5) - 1;
    pointer.base  = (uint32_t)&entries;
}

void GDT::load() {
    gdt_flush((uint32_t)&pointer);
}

void GDT::set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){
  entries[num].base_low = base & 0xFFFF;
  entries[num].base_middle = base >> 16 & 0xFF;
  entries[num].base_high = base >> 24 & 0xFF;

  entries[num].limit_low = (limit & 0xFFFF);

  entries[num].access = access;

  entries[num].granularity = (limit >> 16) & 0x0F; // Get bits 16-19
  entries[num].granularity |= (gran & 0xF0);
}