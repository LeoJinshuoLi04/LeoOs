//Phyical Memory Manager
#include "pmm.hpp"
#include <stddef.h>
#include "terminal.hpp"

extern "C" uint32_t _kernel_end;
extern uint32_t g_kernelResourcesEnd;
extern Terminal* global_terminal;

void PMM::init(multiboot_info* mbi) {
    // Parse multiboot memory map to determine total RAM to initialise bitmap with
    uint64_t memoryEnd{};
    auto mmapEnd = mbi->mmap_addr + mbi->mmap_length;
    auto entryAddress = mbi->mmap_addr;
    while(entryAddress < mmapEnd){
      auto entry = reinterpret_cast<multiboot_mmap_entry*>(entryAddress);
      if(entry->type == 1 && (entry->addr + entry->len) > memoryEnd) memoryEnd = entry->addr + entry->len;
      entryAddress += entry->size + sizeof(entry->size); //size is considered a header and is not include in size for multiboot 1
    }

    max_blocks = memoryEnd / PMM_PAGE_SIZE;
    used_blocks = max_blocks;

    uint32_t bitmapSize = (max_blocks + 31) / 32;
    
    bitmap = reinterpret_cast<uint32_t*>(g_kernelResourcesEnd);
    for(uint32_t i = 0; i< bitmapSize; ++i){ //clear bitmap;
      bitmap[i] = 0xFFFF'FFFF;
    }

    entryAddress = mbi->mmap_addr;
    while(entryAddress < mmapEnd){
      auto entry = reinterpret_cast<multiboot_mmap_entry*>(entryAddress);
      if(entry->type == 1){\
        free_region(entry->addr, entry->len);
      }
      entryAddress += entry->size + sizeof(entry->size); //size is considered a header and is not include in size for multiboot 1
    }

    g_kernelResourcesEnd += bitmapSize*4 - 0xC000'0000;
    set_region(0, g_kernelResourcesEnd);

    set_region(reinterpret_cast<uint32_t>(&mbi), sizeof(multiboot_info));

    set_region(mbi->mmap_addr, mbi->mmap_length);
}

void PMM::free_region(uint32_t start, uint32_t len){
  auto begin = start/PMM_PAGE_SIZE;
  auto end = (start + len + PMM_PAGE_SIZE -1)/PMM_PAGE_SIZE;
  for(auto block = begin; block< end; ++block){
    clear_bit(block);
  }
}

void PMM::set_region(uint32_t start, uint32_t len){
  auto begin = start/PMM_PAGE_SIZE;
  auto end = (start + len + PMM_PAGE_SIZE -1)/PMM_PAGE_SIZE;
  for(auto block = begin; block< end; ++block){
    set_bit(block);
  }
}

void* PMM::alloc_block(){
  //start checking from last free block
  for (uint32_t i = last_free_bit /32 ; i < max_blocks / 32; i++) {
        // Optimization: If the whole uint32 is 0xFFFFFFFF, it's full; skip it
        if (bitmap[i] != 0xFFFF'FFFF) {
            for (uint32_t j = 0; j < 32; j++) {
                uint32_t bit = i * 32 + j;
                if (!test_bit(bit)) {
                    set_bit(bit);
                    last_free_bit = bit+1;
                    return (void*)(bit * PMM_PAGE_SIZE);
                }
            }
        }
    }
    global_terminal->write("PMM: Out of memory!\n"); //TODO: implement exception for this instead of just write;
    return nullptr;
};

uint32_t PMM::get_capacity() const {
  return max_blocks * PMM_PAGE_SIZE;
};

uint32_t PMM::get_used() const {
  return used_blocks * PMM_PAGE_SIZE;
};

void PMM::free_block(void* ptr){
  uint32_t addr = (uint32_t)ptr;
  uint32_t bit = addr / PMM_PAGE_SIZE;
  clear_bit(bit);
};

void PMM::set_bit(uint32_t bit){
  if(test_bit(bit)) return;
  bitmap[bit / 32] |= (1 << (bit % 32));
  ++used_blocks;
};

void PMM::clear_bit(uint32_t bit){
  if(last_free_bit > bit) last_free_bit = bit;
  if(test_bit(bit)){
    bitmap[bit / 32] &= ~(1 << (bit % 32));
    --used_blocks;
  }
};

bool PMM::test_bit(uint32_t bit) const {
  return bitmap[bit / 32] & (1 << (bit % 32));
};