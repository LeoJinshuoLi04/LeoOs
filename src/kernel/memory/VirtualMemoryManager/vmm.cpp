#include "vmm.hpp"
#include "terminal.hpp"
#include "pmm.hpp"

extern Terminal* global_terminal;

extern "C" void load_page_directory(uint32_t* pd_addr);
extern "C" uint32_t _kernel_end;

constexpr uint32_t KERNEL_VIRTUAL_BASE = 0xC0000000;

inline uint32_t virtualFromPhysical(uint32_t physicalAddress){
  return physicalAddress + KERNEL_VIRTUAL_BASE;
}

inline uint32_t physicalFromVirtual(uint32_t virtualAddress){
  return virtualAddress - KERNEL_VIRTUAL_BASE;
}

VMM::VMM(PMM* pmm) : pmm(pmm){
  uint32_t allocatedAddress = reinterpret_cast<uint32_t>(pmm->alloc_block());
  pageDirectory = reinterpret_cast<PageDirectory*>(virtualFromPhysical(allocatedAddress));
};

void VMM::init(){
  for(auto& entry : pageDirectory->entries){
    entry = {};
  }
  // map reserved 16MB of RAM to top 1GB of virtual memory (for kernel use)
  for(uint32_t i = 0; i< 16 * 1024 * 1024; i+= PMM_PAGE_SIZE){
    mapPage(0xC000'0000 | i, i, {1,1}); // present + writable
  }

  // // create temporary identiy mapping for kernel code to allow jump to virtual kernel address space
  for (auto i = 0; i< 16 * 1024 * 1024; i+= PMM_PAGE_SIZE){
    mapPage(i,i, {1,1}); // present + writable
  }

  uint32_t physicalPageDirectory = physicalFromVirtual(reinterpret_cast<uint32_t>(pageDirectory));
  // call assembly stub to load the page directory and enable paging. After this point, all addresses are treated as virtual addresses
  load_page_directory(reinterpret_cast<uint32_t*>(physicalPageDirectory));
  // // delete temporary identity mapping : each table holds 1024 x 4KB = 4MB, so clear 4 entries
  for(auto i = 0; i< 4; i++){
    pageDirectory->entries[i] = {};
  }
  // // flush CPU TLB by reloading page directory
  load_page_directory(reinterpret_cast<uint32_t*>(physicalPageDirectory));
};

void VMM::mapPage(uint32_t vAddress, uint32_t pAddress, VMMFlags flags){
  using namespace VMMUtils;
  uint32_t pd_index = get_pd_index(vAddress);
  PageDirectoryEntry& pageDirectoryEntry = pageDirectory->entries[pd_index];
  if(!pageDirectoryEntry.present){
    uint32_t physicalAllocatedAddress = reinterpret_cast<uint32_t>(pmm->alloc_block());
    auto* newTable = reinterpret_cast<PageTable*>(virtualFromPhysical(physicalAllocatedAddress));

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

void VMM::unmapPage(uint32_t vAddress){
  using namespace VMMUtils;
  uint32_t pd_index = get_pd_index(vAddress);
  auto directoryEntry = pageDirectory->entries[pd_index];
  if(!directoryEntry.present) return; // No page table, so nothing to unmap
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