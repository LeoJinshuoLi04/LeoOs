#include "HeapAllocator.hpp"
#include "terminal.hpp"

static constexpr uint32_t HEADER_POINTER_SIZE = (sizeof(HeapAllocatorHeader*) * 2);
static constexpr uint32_t HEADER_SIZE = sizeof(HeapAllocatorHeader) - HEADER_POINTER_SIZE;

HeapAllocator::HeapAllocator(){}

void HeapAllocator::init(){
  heapStart = 0xD000'0000;
  heapEnd = heapStart + PMM_PAGE_SIZE;

  const uint32_t allocatedAddress = reinterpret_cast<uint32_t>(physicalMemoryManager.alloc_block());
  virtualMemoryManager.mapPage(heapStart, allocatedAddress, {1,1});
  start = reinterpret_cast<HeapAllocatorHeader*>(heapStart);

  start->isFree = true;
  start->size = 4096 - HEADER_SIZE;
  start->next = nullptr;
  start->prev = nullptr;
};

void* HeapAllocator::lalloc(uint32_t requested_size) {
  uint32_t alignedSize = (requested_size + 7) & ~7;
  uint32_t requiredSize = alignedSize + sizeof(HeapAllocatorHeader);

  HeapAllocatorHeader* current = start;
  int n = 0;
  while (current != nullptr) {
    if (current->isFree && current->size >= requiredSize) {
        
        uint32_t new_header_addr = reinterpret_cast<uint32_t>(current) 
                                    - HEADER_POINTER_SIZE
                                    + requiredSize;

        HeapAllocatorHeader* next = reinterpret_cast<HeapAllocatorHeader*>(new_header_addr);
        
        // Set up the new free block
        next->size = current->size - requiredSize + HEADER_POINTER_SIZE;
        next->isFree = true;
        next->next = current->next;
        next->prev = current;

        // Update current block
        current->next = next;
        current->size = alignedSize;
        current->isFree = false;

        if(current == start) start = current->next;
        return reinterpret_cast<void*>(reinterpret_cast<uint32_t>(current) + HEADER_SIZE);
    }

    if (current->next == nullptr) {
        auto* next = expand(requiredSize);
        current->next = next;
        next->prev = current;
    }

    current = current->next;
  }
  return nullptr;
}

HeapAllocatorHeader* HeapAllocator::expand(uint32_t size){
  auto spaceNeeded = size + sizeof(HeapAllocatorHeader) - sizeof(HeapAllocatorHeader*);
  int pagesNeeded = (spaceNeeded + PMM_PAGE_SIZE - 1) / PMM_PAGE_SIZE;
  auto newBlock = reinterpret_cast<HeapAllocatorHeader*>(heapEnd);
  for(auto i = 0; i< pagesNeeded; ++i){
    const uint32_t allocatedAddress = reinterpret_cast<uint32_t>(physicalMemoryManager.alloc_block());
    virtualMemoryManager.mapPage(heapEnd, allocatedAddress, {1,1});
    heapEnd += PMM_PAGE_SIZE;
  }
  newBlock->isFree = true;
  newBlock->next = nullptr;
  newBlock->size = (pagesNeeded * PMM_PAGE_SIZE) - HEADER_SIZE;
  return newBlock;
}

void HeapAllocator::lfree(void* data) {
    if (!data) return;
    auto* curr = reinterpret_cast<HeapAllocatorHeader*>(
        reinterpret_cast<uint8_t*>(data) - HEADER_SIZE
    );
    curr->isFree = true;

    HeapAllocatorHeader* prev = nullptr;
    HeapAllocatorHeader* next = start;

    while (next != nullptr && next < curr) {
        prev = next;
        next = next->next;
    }

    curr->next = next;
    curr->prev = prev;

    if (prev) prev->next = curr;
    else start = curr;

    if (next) next->prev = curr;

    if (next != nullptr) {
        uint32_t end = reinterpret_cast<uint32_t>(curr) + HEADER_SIZE + curr->size;
        if (end == reinterpret_cast<uint32_t>(next)) {
            curr->size += HEADER_SIZE + next->size;
            curr->next = next->next;
            if (curr->next) curr->next->prev = curr;
        }
    }

    if (prev != nullptr) {
        uint32_t end = reinterpret_cast<uint32_t>(prev) + HEADER_SIZE + prev->size;
        if (end == reinterpret_cast<uint32_t>(curr)) {
            prev->size += HEADER_SIZE + curr->size;
            prev->next = curr->next;
            if (prev->next) prev->next->prev = prev;
        }
    }
}

HeapAllocator heapAllocator;