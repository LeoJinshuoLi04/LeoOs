#pragma once

#include <stdint.h>
#include "src/arch/i386/multiboot.hpp"

class PMM {
private:
    uint32_t* bitmap;      // Pointer to the bit array
    uint32_t  max_blocks;  // Total number of 4KB blocks
    uint32_t  used_blocks; // Currently allocated blocks
    uint32_t  last_free_block; // Optimization: Start searching for free blocks from here

public:
    PMM() : bitmap(nullptr), max_blocks(0), used_blocks(0), last_free_block(0) {}
    void init(multiboot_info* mbi);
    void free_region(uint32_t start, uint32_t len);
    void set_region(uint32_t start, uint32_t len);
    
    // Core functions
    void* alloc_block();   // Find a free 4KB page
    void  free_block(void* ptr);

    //accessors
    uint32_t get_capacity() const;
    uint32_t get_used() const;
    
    // Internal helpers
    void set_bit(uint32_t bit);
    void clear_bit(uint32_t bit);
    bool test_bit(uint32_t bit) const;
};