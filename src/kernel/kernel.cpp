#include "terminal.hpp"
#include "src/arch/i386/GlobalDescriptorTable/gdt.hpp"
#include "src/arch/i386/InterruptDescriptorTable/idt.hpp"
#include "src/arch/i386/ProgrammableInterruptController/pic.hpp"
#include "shell.hpp"
#include "src/arch/i386/multiboot.hpp"
#include "pmm.hpp"

Terminal* global_terminal = nullptr; // Global pointer

extern "C" void enable_interrupts();
extern "C" uint32_t _kernel_start;


extern "C" void kernel_main(uint32_t magic, multiboot_info* mbi) {
    Shell shell;
    Terminal T(shell);
    global_terminal = &T; // Point the global to our live object
    if (magic != 0x2BADB002) {
        T.write("Error: Invalid Magic Number\n");
        return;
    }
    GDT gdt;
    gdt.load();
    IDT idt;
    idt.load();
    pic_remap();
    enable_interrupts();
    PMM pmm;
    pmm.init(mbi);
    T.write("Leo OS: ");
    T.write_dec(pmm.get_used()/1024);
    T.write("/");
    T.write_dec(pmm.get_capacity()/1024);
    T.write(" KB RAM Used\n");
    T.write(">> ");
    while(1);
}