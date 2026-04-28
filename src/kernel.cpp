#include "terminal.hpp"
#include "gdt.hpp"
#include "idt.hpp"
#include "pic.hpp"
#include "shell.hpp"
#include "multiboot.hpp"
#include "pmm.hpp"

Terminal* global_terminal = nullptr; // Global pointer

extern "C" void enable_interrupts();
extern "C" uint32_t _kernel_start;
extern "C" uint32_t _kernel_end;

//memory addresses
uint32_t kernel_start = (uint32_t)&_kernel_start;
uint32_t kernel_end = (uint32_t)&_kernel_end;
uint32_t free_ram_start = kernel_end + 4096;

extern "C" void kernel_main(uint32_t magic, multiboot_info* mbi) {
    Shell shell;
    Terminal T(shell);
    global_terminal = &T; // Point the global to our live object
    if (magic != 0x2BADB002) {
        T.write("Error: Invalid Magic Number\n");
        return;
    }
    PMM pmm;
    pmm.init(mbi, &kernel_end);
    T.write("Leo OS: ");
    T.write_dec(pmm.get_used());
    T.write("/");
    T.write_dec(pmm.get_capacity());
    T.write(" B RAM Used\n");
    GDT gdt;
    gdt.load();
    IDT idt;
    idt.load();
    pic_remap();
    enable_interrupts();


    while(1);
}