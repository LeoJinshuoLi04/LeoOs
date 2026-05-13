.section .text
.global timerInterruptHandler

.extern pitHandler 

timerInterruptHandler:

    # 1. Save the CPU state of the current task
    # pushal pushes EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
    pushal          
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    # 2. Load Kernel Data Segments (0x10 is typically the GDT data selector)
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es

    # 3. Pass the current stack pointer to the C++ scheduler
    # The return value (new ESP) will be in %eax
    pushl %esp
    call pitHandler

    # 4. THE SWITCH: Load the stack pointer of the NEW task
    movl %eax, %esp


    # 5. Restore the state of the NEW task
    popl %gs
    popl %fs
    popl %es
    popl %ds
    popal
    
    # 6. Send End of Interrupt (EOI) to the Master PIC
    # We do this right before iret
    pushl %eax          # Save %eax (new ESP) while we use it for OUT
    movb $0x20, %al        
    outb %al, $0x20        
    popl %eax
    
    # 7. Return to the new task
    iret
