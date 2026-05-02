#include <stdint.h>

struct [[gnu::packed]] PageTableEntry {
    uint32_t present    : 1;   // Is this page in RAM?
    uint32_t writable   : 1;   // 1 = Read/Write, 0 = Read-Only
    uint32_t user       : 1;   // 1 = User mode access, 0 = Kernel only
    uint32_t reserved   : 9;   // Bits for the OS to use
    uint32_t frame_addr : 20;  // The top 20 bits of the physical address
};

struct [[gnu::packed]] alignas(4096) PageTable {
    PageTableEntry entries[1024];
};

struct PageDirectoryEntry {
    uint32_t present    : 1;
    uint32_t writable   : 1;
    uint32_t user       : 1;
    uint32_t write_thru : 1;
    uint32_t cache_dis  : 1;
    uint32_t accessed   : 1;
    uint32_t reserved   : 1;
    uint32_t page_size  : 1; // 0 for 4KB pages
    uint32_t ignored    : 4;
    uint32_t table_addr : 20; // Physical address of the Page Table
};

struct alignas(4096) PageDirectory {
    PageDirectoryEntry entries[1024];
};