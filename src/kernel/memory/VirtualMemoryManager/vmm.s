.global load_page_directory
load_page_directory:
    mov 4(%esp), %eax   # Get pd_addr argument from stack
    mov %eax, %cr3      # Load Page Directory Physical Address

    mov %cr0, %eax
    or $0x80000000, %eax
    mov %eax, %cr0      # PAGING IS NOW ON!
    ret