//Phyical Memory Manager
#include "pmm.hpp"
#include <stddef.h>


constexpr int PAGE_SIZE {4096}; //4Kb page size;

void PMM::init(multiboot_info* mbi, uint32_t* kernel_end) {
    bitmap = kernel_end;
    
    // 1. Calculate total possible blocks based on RAM reporting
    uint32_t total_mem = (mbi->mem_upper + 1024) * 1024; 
    max_blocks = total_mem / PAGE_SIZE;
    
    // 2. Start by marking everything as USED (1)
    for (uint32_t i = 0; i < max_blocks; i++) {
        set_bit(i);
    }
    used_blocks = max_blocks;

    // 3. Parse the mmap and FREE (0) the Type 1 regions
    auto* entry = (multiboot_mmap_entry*)mbi->mmap_addr;
    while ((uint32_t)entry < mbi->mmap_addr + mbi->mmap_length) {
        if (entry->type == 1) { // 1 = Available RAM
            free_region(entry->addr, entry->len);
        }
        // Move to next entry using the 'size' field
        entry = (multiboot_mmap_entry*)((uint32_t)entry + entry->size + sizeof(entry->size));
    }

    // 4. Protect the Kernel and BIOS (0x0 to end of kernel + bitmap)
    // You'll need to pass the kernel_end address or use the linker symbol
    extern uint32_t _kernel_end;
    uint32_t kernel_resources_end = (uint32_t)&_kernel_end + 4096; // Kernel + Bitmap page
    set_region(0, kernel_resources_end);
}

void PMM::free_region(uint32_t start, uint32_t len){
  auto begin = start/PAGE_SIZE;
  auto end = (start + len + PAGE_SIZE -1)/PAGE_SIZE;
  for(auto block = begin; block< end; ++block){
    clear_bit(block);
    --used_blocks;
  }
}

void PMM::set_region(uint32_t start, uint32_t len){
  auto begin = start/PAGE_SIZE;
  auto end = (start + len + PAGE_SIZE -1)/PAGE_SIZE;
  for(auto block = begin; block< end; ++block){
    set_bit(block);
    ++used_blocks;
  }
}

void* PMM::alloc_block(){
  //start checking from last free block
  for (uint32_t i = last_free_block; i < max_blocks / 32; i++) {
        // Optimization: If the whole uint32 is 0xFFFFFFFF, it's full; skip it
        if (bitmap[i] != 0xFFFFFFFF) {
            for (int j = 0; j < 32; j++) {
                int bit = i * 32 + j;
                if (!test_bit(bit)) {
                    set_bit(bit);
                    used_blocks++;
                    last_free_block = bit+1;
                    return (void*)(bit * PAGE_SIZE);
                }
            }
        }
    }
    return nullptr;
};

uint32_t PMM::get_capacity() const {
  return max_blocks * PAGE_SIZE;
};

uint32_t PMM::get_used() const {
  return used_blocks * PAGE_SIZE;
};



void PMM::free_block(void* ptr){
  uint32_t addr = (uint32_t)ptr;
  uint32_t bit = addr / PAGE_SIZE;
  clear_bit(bit);
  used_blocks--;
};

void PMM::set_bit(uint32_t bit){
  bitmap[bit / 32] |= (1 << (bit % 32));
};

void PMM::clear_bit(uint32_t bit){
  last_free_block = bit;
  bitmap[bit / 32] &= ~(1 << (bit % 32));
};

bool PMM::test_bit(uint32_t bit) const {
  return bitmap[bit / 32] & (1 << (bit % 32));
};