#pragma once
#include "pageDirectory.hpp"
#include "pmm.hpp"

struct [[gnu::packed]] VMMFlags {
  uint32_t present : 1;
  uint32_t writable : 1;
  //TODO: other flags will be added in the future
};

class VMM {
  private:
    PageDirectory* pageDirectory;
    PMM* pmm;
  public:
    VMM() = delete;
    VMM(PMM* pmm);
    void init();
    void mapPage(uint32_t vAddress, uint32_t pAddress, VMMFlags flags);
    void unmapPage(uint32_t vAddress);
    uint32_t translate(uint32_t vAddress);
};

namespace VMMUtils {
  // Get the top 10 bits
  constexpr uint32_t get_pd_index(uintptr_t vAddress){
      return vAddress >> 22;
  }

  // Get the middle 10 bits
  constexpr uint32_t get_pt_index(uintptr_t vAddress) {
      return (vAddress >> 12) & 0x3FF;
  }

  // Turn that 20-bit field back into a full address
  constexpr uintptr_t get_addr_from_entry(uint32_t entry_addr_field) {
      return entry_addr_field << 12;
  }
}