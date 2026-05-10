#include "vmm.hpp"
#include "terminal.hpp"
#include "pmm.hpp"

extern Terminal* global_terminal;

extern "C" void load_page_directory(uint32_t* pd_addr);

constexpr uint32_t KERNEL_VIRTUAL_BASE = 0xC0000000;

VMM::VMM(){};

void VMM::init(){
  uint32_t allocatedAddress = reinterpret_cast<uint32_t>(physicalMemoryManager.alloc_block());
  pageDirectory = reinterpret_cast<PageDirectory*>(allocatedAddress + KERNEL_VIRTUAL_BASE);
  uint32_t vmmEnd = allocatedAddress + 4096; // 4Kb page

  for(auto& entry : pageDirectory->entries){
    entry = {};
  }

  uint32_t physicalPageDirectoryAddress = reinterpret_cast<uint32_t>(pageDirectory) - KERNEL_VIRTUAL_BASE;
  pageDirectory->entries[1023].table_addr =  physicalPageDirectoryAddress >> 12;
  pageDirectory->entries[1023].present = true;
  pageDirectory->entries[1023].writable = true;

  // map reserved 16MB of RAM to top 1GB of virtual memory (for kernel use)
  for(uint32_t i = 0; i< vmmEnd; i+= PMM_PAGE_SIZE){
    initMapPage(0xC000'0000 | i, i, {1,1}); // present + writable
  }
  // call assembly stub to load the page directory and enable paging. After this point, all addresses are treated as virtual addresses
  load_page_directory(reinterpret_cast<uint32_t*>(physicalPageDirectoryAddress));
};

void VMM::initMapPage(uint32_t vAddress, uint32_t pAddress, VMMFlags flags){
  using namespace VMMUtils;
  uint32_t pageDirectoryIndex = get_pd_index(vAddress);
  PageDirectoryEntry& pageDirectoryEntry = pageDirectory->entries[pageDirectoryIndex];
  if(!pageDirectoryEntry.present){
    uint32_t physicalAllocatedAddress = reinterpret_cast<uint32_t>(physicalMemoryManager.alloc_block());
    auto* newTable = reinterpret_cast<PageTable*>(physicalAllocatedAddress + KERNEL_VIRTUAL_BASE);
    for(auto& entry : newTable->entries){
      entry = {};
    }
    pageDirectoryEntry.present = 1;
    pageDirectoryEntry.table_addr = reinterpret_cast<uint32_t>(physicalAllocatedAddress) >> 12;
    pageDirectoryEntry.writable = flags.writable;
  };

  PageTable* pageTable = reinterpret_cast<PageTable*>(get_addr_from_entry(pageDirectoryEntry.table_addr));
  uint32_t pt_index = get_pt_index(vAddress);
  PageTableEntry& pageTableEntry = pageTable->entries[pt_index];
  if(!pageTableEntry.present){
    pageTableEntry.present = 1;
    pageTableEntry.writable = flags.writable;
    pageTableEntry.frame_addr = pAddress >> 12;
  }else{
    global_terminal->write("Overriding existing mapping with virtual address: 0x");
    global_terminal->write_hex(vAddress);
    global_terminal->write("\n");
  }
};

void VMM::mapPage(uint32_t vAddress, uint32_t pAddress, VMMFlags flags) {
  using namespace VMMUtils;
    uint32_t pageDirectoryIndex = get_pd_index(vAddress);
    
    // 1. Access the PDE using the Recursive Page Directory address
    // This is the last 4KB of the 4MB wormhole: 0xFFFFF000
    PageDirectoryEntry* pageDirectoryEntry = reinterpret_cast<PageDirectoryEntry*>((0xFFFFF000 + (pageDirectoryIndex * 4)));

    if (!pageDirectoryEntry->present) {
        // TABLE NOT PRESENT: We must create it.
        uint32_t pageTableAddress = reinterpret_cast<uint32_t>(physicalMemoryManager.alloc_block());
        pageDirectoryEntry->table_addr = pageTableAddress >> 12;
        pageDirectoryEntry->present = true;
        pageDirectoryEntry->writable = true;

        // ZERO the new table using the wormhole
        PageTable* pageTable = reinterpret_cast<PageTable*>(0xFFC00000 + (pageDirectoryIndex * 0x1000));
        for(auto& entry : pageTable->entries) entry = {};
    }

    PageTableEntry* pageTableEntry = reinterpret_cast<PageTableEntry*>(0xFFC00000 + ((vAddress >> 12) * 4));
    
    pageTableEntry->frame_addr = pAddress >> 12;
    pageTableEntry->present = true;
    pageTableEntry->writable = flags.writable;

    // 3. CRITICAL: Flush the TLB for this address
    asm volatile("invlpg (%0)" :: "r" (vAddress) : "memory");
}

void VMM::unmapPage(uint32_t vAddress){
  using namespace VMMUtils;
  uint32_t pd_index = get_pd_index(vAddress);
  auto directoryEntry = pageDirectory->entries[pd_index];
  if(!directoryEntry.present) return;
  uint32_t pt_index = get_pt_index(vAddress);
  auto* pageTable = reinterpret_cast<PageTable*>(get_addr_from_entry(directoryEntry.table_addr));
  pageTable->entries[pt_index] = {}; // Clear the entry to unmap
};

uint32_t VMM::translate(uint32_t vAddress){
  using namespace VMMUtils;
  uint32_t pd_index = get_pd_index(vAddress);
  auto directoryEntry = pageDirectory->entries[pd_index];
  if(!directoryEntry.present) return 0; //TODO: maybe better return value, Page Table not present
  uint32_t pt_index = get_pt_index(vAddress);
  auto* pageTable = reinterpret_cast<PageTable*>(get_addr_from_entry(directoryEntry.table_addr));
  if(!pageTable->entries[pt_index].present) return 0; //TODO: same as above, Page not present

  return (pageTable->entries[pt_index].frame_addr << 12) | (vAddress & 0xFFF); // Combine frame address with page offset
};

VMM virtualMemoryManager;