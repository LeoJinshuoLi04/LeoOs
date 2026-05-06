.macro ISR_NOERRCODE num
  .global isr\num
  isr\num:
    pushl $0             # Push a dummy error code
    pushl $\num           # Push the interrupt number
    jmp isr_common_stub
.endm

.macro ISR_ERRCODE num
  .global isr\num
  isr\num:
    pushl $\num           # Push the interrupt number (error code already pushed by CPU)
    jmp isr_common_stub
.endm

# Define the first few exceptions
ISR_NOERRCODE 0   # Division by Zero
ISR_NOERRCODE 1   # Debug
ISR_NOERRCODE 2   
ISR_NOERRCODE 3   
ISR_NOERRCODE 4   
ISR_NOERRCODE 5   
ISR_NOERRCODE 6   
ISR_NOERRCODE 7   
ISR_ERRCODE   8   # Double Fault
ISR_NOERRCODE 9   
ISR_NOERRCODE 10   
ISR_NOERRCODE 11   
ISR_NOERRCODE 12   
ISR_NOERRCODE 13   
ISR_NOERRCODE 14   
ISR_NOERRCODE 15   
ISR_NOERRCODE 16   
ISR_NOERRCODE 17   
ISR_NOERRCODE 18   
ISR_NOERRCODE 19   
ISR_NOERRCODE 20   
ISR_NOERRCODE 21   
ISR_NOERRCODE 22   
ISR_NOERRCODE 23   
ISR_NOERRCODE 24   
ISR_NOERRCODE 25   
ISR_NOERRCODE 26   
ISR_NOERRCODE 27   
ISR_NOERRCODE 28   
ISR_NOERRCODE 29   
ISR_NOERRCODE 30   
ISR_NOERRCODE 31     
ISR_NOERRCODE 32  # Timer
ISR_NOERRCODE 33
# ... you'll eventually add all 32 here

.extern isr_handler
isr_common_stub:
    pusha                # Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
    mov %ds, %ax         # Lower 16-bits of eax = ds
    push %eax            # Save the data segment descriptor

    mov $0x10, %ax       # Load the Kernel Data Segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    call isr_handler     # Jump to our C++ handler!

    pop %eax             # Reload the original data segment descriptor
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    popa                 # Pops edi, esi, etc
    add $8, %esp         # Cleans up the pushed error code and pushed ISR number
    iret                 # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP
