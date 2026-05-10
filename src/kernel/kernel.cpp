#include "terminal.hpp"
#include "src/arch/i386/GlobalDescriptorTable/gdt.hpp"
#include "src/arch/i386/InterruptDescriptorTable/idt.hpp"
#include "src/arch/i386/ProgrammableInterruptController/pic.hpp"
#include "shell.hpp"
#include "src/arch/i386/multiboot.hpp"
#include "pmm.hpp"
#include "vmm.hpp"

Terminal* global_terminal = nullptr; // Global pointer

extern "C" void enable_interrupts();
extern "C" uint32_t _kernel_start;
extern "C" uint32_t _kernel_end;

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
    pic_remap();
    enable_interrupts();
    T.write("Leo OS: ");
    T.write_dec(physicalMemoryManager.get_used()/1024);
    T.write("/");
    T.write_dec(physicalMemoryManager.get_capacity()/1024);
    T.write(" KB RAM Used\n");
    T.write(">> ");
    while(1);
}