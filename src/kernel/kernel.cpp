#include "terminal.hpp"
#include "src/arch/i386/GlobalDescriptorTable/gdt.hpp"
#include "src/arch/i386/InterruptDescriptorTable/idt.hpp"
#include "src/arch/i386/ProgrammableInterruptController/pic.hpp"
#include "shell.hpp"
#include "src/arch/i386/multiboot.hpp"
#include "pmm.hpp"
#include "vmm.hpp"
#include "HeapAllocator.hpp"

Terminal* global_terminal = nullptr; // Global pointer

extern "C" void enable_interrupts();

void printMemory(){
    global_terminal->write_dec(physicalMemoryManager.get_used()/1024);
    global_terminal->write("/");
    global_terminal->write_dec(physicalMemoryManager.get_capacity()/1024);
    global_terminal->write(" KB RAM Used\n");
}

extern "C" void kernel_main(uint32_t magic, multiboot_info* mbi) {
    Shell shell;
    Terminal T(shell);
    global_terminal = &T; // Point the global to our live object
    if (magic != 0x2BADB002) {
        T.write("Error: Invalid Magic Number. Received: 0x");
        T.write_hex(magic);
        T.write("\n");
        return;
    }
    globalDescriptorTable.load();
    interruptDescriptorTable.load();
    physicalMemoryManager.init(mbi);
    virtualMemoryManager.init();
    heapAllocator.init();
    pic_remap();
    enable_interrupts();
    T.write("Leo OS: ");
    printMemory();
    T.write(">> ");
    while(1);
}
