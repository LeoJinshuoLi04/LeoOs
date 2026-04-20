LeoOS Kernel
LeoOS is a minimalist 32-bit x86 kernel written in C++ and Assembly. The project focuses on building a stable foundation for a protected-mode operating system, implementing core CPU data structures and hardware abstraction layers.

Technical Overview
Boot and Memory Management
Multiboot Compliance: The kernel utilizes a custom Assembly entry point compatible with the Multiboot specification, allowing it to be loaded by GRUB or QEMU.

Global Descriptor Table (GDT): Configured a flat 32-bit memory model. This establishes kernel-level code and data segments, which are necessary for memory protection and future user-mode transitions.

Linker Configuration: A custom linker script manages the physical and virtual address layout, ensuring the Multiboot header resides within the first 8KB of the binary.

Hardware Abstraction
VGA Text Driver: An object-oriented C++ driver for the 0xB8000 video buffer. It supports character-attribute mapping and features a software-based scrolling algorithm to handle vertical overflow.

Port I/O: Implementation of inline assembly wrappers for inb and outb instructions, enabling safe communication with motherboard hardware peripherals.

Interrupt Architecture
Interrupt Descriptor Table (IDT): A 256-entry table that facilitates the mapping of hardware IRQs and software exceptions to specific handler routines.

Interrupt Service Routines (ISR): Assembly stubs serve as low-level bridges, preserving CPU state and registers before delegating execution to high-level C++ logic.

PIC Remapping: The Programmable Interrupt Controllers (8259A) have been remapped to shift hardware interrupts (IRQ 0-15) to the 0x20-0x2F range, preventing conflicts with CPU-internal exceptions.

Fault Tolerance: Includes specialized handlers for synchronous CPU exceptions (e.g., Division by Zero) to ensure system stability during runtime errors.

Project Structure
src/boot.s: Bootstrapping logic and stack initialization.

src/interrupts.s: Low-level assembly wrappers for the interrupt stack.

src/terminal.cpp: Implementation of the VGA text-mode driver.

src/gdt.cpp / src/idt.cpp: CPU table initialization and gate configuration.

src/pic.cpp: Logic for remapping and managing the PIC.

src/kernel.cpp: Kernel entry point and hardware initialization sequence.

Building and Execution
Prerequisites
i686-elf-gcc cross-compiler toolchain

GNU Make

QEMU (i386)

Usage
To compile the kernel and launch the environment:

Bash

make clean
make run
