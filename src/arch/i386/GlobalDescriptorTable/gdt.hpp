#pragma once
#include <stdint.h>

struct gdt_entry {
    uint16_t limit_low;      //specifies length of segment combined with granularity
    uint16_t base_low;       //specifies starting address of segment with base_middle and base_high
    uint8_t  base_middle;
    uint8_t  access;         // see README for details
    uint8_t  granularity;    // comprised of size bit and granularity bit
                             // granularity bit = 1 means limit is in 4KB blocks else 1 byte
                             // size bit = 1 means 32-bit , = 0 means 16 bit segment
    uint8_t  base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

class GDT {
public:
    GDT();
    void load();
private:
    void set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
    gdt_entry entries[5];
    gdt_ptr pointer;
};