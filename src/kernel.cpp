#include "terminal.hpp"
#include "gdt.hpp"
#include "idt.hpp"
#include "pic.hpp"
#include "shell.hpp"
#include "multiboot.hpp"

Terminal* global_terminal = nullptr; // Global pointer

extern "C" void enable_interrupts();

extern "C" void kernel_main(uint32_t magic, multiboot_info* mbi) {
    Shell shell;
    Terminal T(shell);
    global_terminal = &T; // Point the global to our live object
    if (magic != 0x2BADB002) {
        T.write("Error: Invalid Magic Number\n");
        return;
    }
    uint32_t total_mb = (mbi->mem_upper / 1024) + 1;
    T.write("Leo OS: ");
    T.write_dec(total_mb);
    T.write(" MB\n>> ");
    GDT gdt;
    gdt.load();
    IDT idt;
    idt.load();
    pic_remap();
    enable_interrupts();


    while(1);
}