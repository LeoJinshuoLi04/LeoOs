PIC Remapping: The Programmable Interrupt Controllers (8259A) have been remapped to shift hardware interrupts (IRQ 0-15) to the 0x20-0x2F range, preventing conflicts with CPU-internal exceptions.

Basically when the first intel chips had only a few exceptions, and so the PIC just went above that. Then they introduced more CPU exceptions so now theres overlap between hardware and CPU interrupts, so we need to remap to differentiate between the two.
