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
ISR_NOERRCODE 8   # Double Fault (Has error code, but we'll simplify for now)
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
