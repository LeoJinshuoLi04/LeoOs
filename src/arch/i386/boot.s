# Constants for the Multiboot Header
.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

# Multiboot header that GRUB/QEMU looks for
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# Reserve 16KiB for the stack
.section .bss
.align 16
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start

_start:
    # Set the stack pointer
    mov $stack_top, %esp

    # reset ebp to 0
    xor %ebp, %ebp

    # 3. Push arguments for kernel_main(magic, mbi_ptr)
    # The Multiboot spec says: magic is in EAX, pointer is in EBX
    # C Calling Convention (cdecl) pushes arguments from right to left
    
    push %ebx      # Argument 2: Multiboot Information Structure pointer
    push %eax      # Argument 1: Magic Number (0x2BADB002)

    # 4. Enter the C++ code
    call kernel_main

    # If the kernel ever returns, just hang the CPU
    cli
1:  hlt
    jmp 1b

.global enable_interrupts
enable_interrupts:
    sti
    ret
