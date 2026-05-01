#ifndef __MULTIBOOT_H
#define __MULTIBOOT_H

#include <stdint.h>

struct multiboot_mmap_entry {
    uint32_t size;        // size of entry in bytes
    uint64_t addr;        // starting physical address on computer
    uint64_t len;         // length of region in bytes
    uint32_t type;        // 1 = Available RAM, other values are reserved
} __attribute__((packed));

struct multiboot_info {
    uint32_t flags;       // bitfield indicating which fields are valid
    uint32_t mem_lower;   // simple, imprecise indicators of lower memory in KB, (under 640KB)
    uint32_t mem_upper;   // simple, imprecise indicators of upper memory in KB, (above 1MB)
    uint32_t boot_device; // which BIOS disk device the OS image was loaded from
    uint32_t cmdline;     // address of args passed to kernel
    uint32_t mods_count;  // extra modules loaded
    uint32_t mods_addr;   // address of modules
    uint32_t syms[4];     
    uint32_t mmap_length; // size of buffer containing mmap entries, in bytes
    uint32_t mmap_addr;   // address of the mmap
    // There are more fields, but we stop here for now
} __attribute__((packed));

#endif