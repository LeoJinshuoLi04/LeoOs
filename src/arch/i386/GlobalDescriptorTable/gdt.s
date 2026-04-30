.global gdt_flush
gdt_flush:
    mov 4(%esp), %eax   # Get the pointer to the GDT from the stack
    lgdt (%eax)         # Load the new GDT pointer

    mov $0x10, %ax      # 0x10 is the offset to our Data Segment (Entry 2)
    mov %ax, %ds        # Load all data segment registers
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    ljmp $0x08, $.flush # 0x08 is the Code Segment (Entry 1). Far jump!
.flush:
    ret
