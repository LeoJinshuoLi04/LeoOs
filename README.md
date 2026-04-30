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
LeoOS/
├── Makefile                
│
└── src/                    # All Source Code
    │
    ├── arch/               # Architecture-specific code (Hardware tied)
    │   └── i386/           # 32-bit x86 specific files
    │       ├── linker.ld   # Linker script
    │       ├── boot.s      # Multiboot header and _start
    │       ├── gdt.s       # Assembly stub for GDT loading
    │       ├── interrupt.s # Assembly stubs for ISRs (pusha, iret)
    │       ├── gdt.cpp     # Global Descriptor Table setup
    │       ├── idt.cpp     # Interrupt Descriptor Table setup
    │       ├── pic.cpp     # 8259 Programmable Interrupt Controller (Remap)
    │       └── ports.cpp   # inb, outb (I/O port communication)
    │
    ├── kernel/             # Architecture-agnostic core OS logic
    │   ├── main.cpp        # kernel_main()
    │   ├── memory/         
    │   │   ├── pmm.cpp     # Physical Memory Manager (Bitmap)
    │   │   ├── vmm.cpp     # (Pending) Virtual Memory Manager (Paging)
    │   │   └── heap.cpp    # (Pending) kmalloc/kfree (Dynamic memory)
    │   ├── cpu/
    │   │   └── isr.cpp     # (Pending) High-level C++ interrupt dispatcher
    │   ├── task/           # (Pending) Scheduler
    │   │   ├── process.cpp # (Pending) Process Control Blocks (PCB)
    │   │   └── sched.cpp   # (Pending) Round-robin or priority scheduler
    │   └── syscall/        # System Calls
    │       └── syscall.cpp # (Pending) Ring 3 to Ring 0 gateway
    │
    ├── drivers/            # Hardware drivers (Ring 0 for now, user space later)
    │   ├── vga/
    │   │   ├── terminal.cpp    # Text mode terminal (what you have now)
    │   │   └── fb.cpp      # (Pending) Framebuffer graphics
    │   ├── input/
    │   │   ├── kbd.cpp     # PS/2 Keyboard
    │   │   └── mouse.cpp   # (Pending) PS/2 Mouse
    │   └── timer/
    │       └── pit.cpp     # (Pending) Programmable Interval Timer (for scheduling)
    │
    ├── lib/                # "klibc" - Kernel's version of standard libraries
    │   ├── string.cpp      # memcpy, memset, strlen
    │   ├── memory.cpp      # placement new operator overloads
    │   └── structures/     # (Pending) More custom data structures
    │       ├── vector.hpp
    │       └── bitmap.hpp  
    │
    └── user/               # User-space programs (Ring 3 - Added much later)
        ├── libc/           # (Pending) A minimal standard library for your apps
        ├── apps/
        │   ├── init/       # (Pending) The first user process (starts the system)
        │   └── shell/      # The interactive command line
        └── gui/
            ├── wm.cpp      # (Pending) Window Manager
            └── desktop.cpp # (Pending) The graphical desktop environment

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
