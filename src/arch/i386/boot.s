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

.section .data
.align 4096
boot_page_directory:
    # Entry 0: Identity Map (0 to 4MB)
    # 0x83 = Present + R/W + 4MB Page Size (PSE)
    .long 0x00000083
    
    # Fill gaps with 0
    .fill 767, 4, 0
    
    # Entry 768: Higher Half Map (3GB mapped to physical 0 to 4MB)
    .long 0x00000083
    
    # Fill the rest of the 1024 entries
    .fill 254, 4, 0

.section .text
.global _start
_start:
    # Save GRUB's registers immediately!
    # ESI and EDI are safe to use as scratch registers here.
    mov %eax, %esi  # ESI now holds the Magic Number
    mov %ebx, %edi  # EDI now holds the MBI pointer

    # Enable PSE (Page Size Extensions) to allow 4MB pages
    mov %cr4, %eax
    or $0x00000010, %eax
    mov %eax, %cr4

    # Load physical address of bootstrap page directory
    # (Subtract 3GB offset because paging is OFF right now)
    mov $(boot_page_directory - 0xC0000000), %ecx
    mov %ecx, %cr3

    # Turn on Paging!
    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0

    # Jump to the Higher Half
    # This absolute jump forces the Instruction Pointer into the 0xC0... range
    lea 4f, %eax
    jmp *%eax

4:
    # --- YOU ARE NOW IN THE HIGHER HALF ---
    # Set stack to virtual address
    mov $stack_top, %esp

    # Push the saved arguments for C++ (Right to Left)
    push %edi # Argument 2: MBI pointer
    push %esi # Argument 1: Magic Number

    # Call your C++ kernel main
    call kernel_main

    cli
1:  hlt
    jmp 1b


.global enable_interrupts
enable_interrupts:
    sti
    ret
