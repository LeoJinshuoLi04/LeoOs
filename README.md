LeoOS Kernel
LeoOS is a minimalist 32-bit x86 kernel written in C++ and Assembly. The project focuses on building a stable foundation for a protected-mode operating system, implementing core CPU data structures and hardware abstraction layers.

Technical Overview
Boot and Memory Management
Multiboot Compliance: The kernel utilizes a custom Assembly entry point compatible with the Multiboot specification, allowing it to be loaded by GRUB or QEMU.

Linker Configuration: A custom linker script manages the physical and virtual address layout, ensuring the Multiboot header resides within the first 8KB of the binary.

Project Structure
LeoOS/
├── Makefile                
│
└── src/                      # All Source Code
    │
    ├── arch/                  # Architecture-specific code (Hardware tied)
    │   └── i386/              # 32-bit x86 specific files
    │       ├── gdt            # Global Descriptor Table setup
    │       ├── idt            # Interrupt Descriptor Table setup
    │       ├── pic            # 8259 Programmable Interrupt Controller (Remap)
    │       ├── linker.ld      # Linker script
    │       ├── boot.s         # Multiboot header and _start
    │       ├── interrupt.s    # Assembly stubs for ISRs (pusha, iret)
    │       ├── multiboot.hpp  # Assembly stubs for ISRs (pusha, iret)
    │       └── ports.cpp      # inb, outb (I/O port communication)
    │
    ├── kernel/                # Architecture-agnostic core OS logic
    │   ├── main.cpp           # kernel_main()
    │   ├── memory/         
    │   │   ├── pmm.cpp        # Physical Memory Manager (Bitmap)
    │   │   ├── vmm.cpp        # Virtual Memory Manager (Paging)
    │   │   └── heap.cpp       # kmalloc/kfree (Dynamic memory)
    │   ├── cpu/
    │   │   └── isr.cpp        # High-level C++ interrupt dispatcher
    │   ├── task/              # (Pending) Scheduler
    │   │   ├── process.cpp    # (Pending) Process Control Blocks (PCB)
    │   │   └── sched.cpp      # (Pending) Round-robin or priority scheduler
    │   └── syscall/           # System Calls
    │       └── syscall.cpp    # (Pending) Ring 3 to Ring 0 gateway
    │
    ├── drivers/               # Hardware drivers (Ring 0 for now, user space later)
    │   ├── vga/
    │   │   ├── terminal.cpp   # Text mode terminal (what you have now)
    │   │   └── fb.cpp         # (Pending) Framebuffer graphics
    │   ├── input/
    │   │   ├── kbd.cpp        # PS/2 Keyboard
    │   │   └── mouse.cpp      # (Pending) PS/2 Mouse
    │   └── timer/
    │       └── pit.cpp        # (Pending) Programmable Interval Timer (for scheduling)
    │
    ├── lib/                   # "klibc" - Kernel's version of standard libraries
    │   ├── string.cpp         # memcpy, memset, strlen
    │   ├── memory.cpp         # placement new operator overloads
    │   └── structures/        # (Pending) More custom data structures
    │       ├── vector.hpp
    │       └── bitmap.hpp  
    │
    └── user/                  # User-space programs (Ring 3 - Added much later)
        ├── libc/              # (Pending) A minimal standard library for your apps
        ├── apps/
        │   ├── init/          # (Pending) The first user process (starts the system)
        │   └── shell/         # The interactive command line
        └── gui/
            ├── wm.cpp         # (Pending) Window Manager
            └── desktop.cpp    # (Pending) The graphical desktop environment

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
