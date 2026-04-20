#include "terminal.hpp"
#include "gdt.hpp"
#include "idt.hpp"
#include "pic.hpp"

Terminal* global_terminal = nullptr; // Global pointer

extern "C" void enable_interrupts();

extern "C" void kernel_main() {
    Terminal T;
    global_terminal = &T; // Point the global to our live object
    T.write("Leo OS\n");
    GDT gdt;
    gdt.load();
    IDT idt;
    idt.load();
    pic_remap();
    enable_interrupts();
    while(1);
}