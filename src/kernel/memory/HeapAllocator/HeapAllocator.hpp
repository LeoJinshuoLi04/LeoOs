#pragma once

#include "vmm.hpp"

struct HeapAllocatorHeader {
  uint32_t size;
  alignas(4) bool isFree;
  HeapAllocatorHeader* prev;
  HeapAllocatorHeader* next;
};

class HeapAllocator {
  private:
    HeapAllocatorHeader* start = nullptr;
    uint32_t heapStart;
    uint32_t heapEnd;
  public:
    HeapAllocator();
    void init();
    HeapAllocatorHeader* expand(uint32_t size);
    void* lalloc(uint32_t size);
    void lfree(void* data);
};

extern HeapAllocator heapAllocator;