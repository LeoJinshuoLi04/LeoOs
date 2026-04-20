my-os/
├── src/
│ ├── boot.s # Assembly entry point (Multiboot header)
│ ├── kernel.cpp # Your C++ logic
│ └── linker.ld # Tells the linker where to put code in memory
├── build/ # Where compiled objects go
└── Makefile # To automate the build/run process

🛠 LeoOS: Project Progress Recap

1. The Bootloader Handshake (The Entry)
   Component: boot.s (Assembly) & linker.ld

Purpose: \* Defines the Multiboot Header so QEMU/GRUB recognizes the file as a kernel.

Sets up the Stack Pointer (esp), which is the literal foundation required for C++ functions to execute.

The Linker Script ensures the kernel is loaded at the 1MB mark in physical memory, keeping it away from BIOS-reserved areas.

2. VGA Text Driver (The Voice)
   Component: terminal.cpp & terminal.hpp

Purpose: \* Provides the first abstraction layer over raw hardware.

Manages the VGA Memory Buffer at 0xB8000.

Implements Software Scrolling by shifting memory blocks when the screen fills up.

Converts ASCII characters into the 16-bit "Character + Attribute" format the hardware expects.

3. Global Descriptor Table - GDT (The Security Guard)
   Component: gdt.cpp & gdt.hpp

Purpose: \* Defines how the CPU perceives memory.

Sets up "Segments" for Kernel Code and Kernel Data.

While we use a "Flat Memory Model" (0 to 4GB), this setup is mandatory for the CPU to enter a stable Protected Mode and prepares the system for future User Space isolation (Ring 3).

4. Interrupt Descriptor Table - IDT (The Nervous System)
   Component: idt.cpp & idt.hpp

Purpose: \* Creates a mapping between specific events and handler functions.

Tells the CPU exactly where to jump when a "Software Exception" (like Division by Zero) or a "Hardware Interrupt" occurs.

Acts as the bridge between asynchronous hardware events and your synchronous C++ code.

5. ISR Handling & Assembly Stubs (The Safety Net)
   Component: interrupts.s & isr.cpp

Purpose: \* Provides the Assembly Wrappers that save the current state of the CPU (registers) before jumping into C++.

Implements the first Exception Handler, allowing the OS to catch crashes (like Division by Zero) gracefully instead of silently rebooting.
