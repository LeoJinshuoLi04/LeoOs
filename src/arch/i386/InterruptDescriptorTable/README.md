Interrupt Architecture
Interrupt Descriptor Table (IDT): A 256-entry table that facilitates the mapping of hardware IRQs and software exceptions to specific handler routines.

Each entry has a flags atrribute specifying the following
bit 7         Present - 1 for interrupt to be triggered
bits 6 & 5    Privilege bits - 00 for kernel access and 11 for user
bit 4         Storage - always 0 to define as system code instead of memory
bits 3 - 0    Gate type - behaviour once inside (can more interrupts be called?)

When a certain interrrupt is called, the gate directs the CPu to the relevant handler code to run.