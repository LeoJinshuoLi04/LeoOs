Global Descriptor Table (GDT): Configured a flat 32-bit memory model. This establishes kernel-level code and data segments, which are necessary for memory protection and future user-mode transitions.

GDT is a custom mapping for different access levels that we have, it is essentially an access control list.
0 - Null descriptor         Faults when we try to access
1 - Kernel's executable     Has the highest access level
2 - Kernel's stack/heap     Read and write
3 - User code segment       User applications
4 - User data segment       User data

It limits access to memory based on access level, which we set as:
bit 7         Present - if it has been set
bits 6 & 5    Privilege bits - 00 for kernel access and 11 for user
bit 4         System - 1 for code/data segments 0 for system segments
bit 3         Executable - 1 if runnable, 0 if data
bit 2         Direction - For data: 0 means grows up, 1 means grows down
              Conforming - For code: 1 means lower privilege can access
bit 1         Read/Write - For code : 1 = readable, never writeable
                           For data: 1 = writeable, 0 = readable
bit 0         Accessed - Set when it is being accessed currently

We are defining a flat structure for our system, meaning all sections have access to all memory. This is the modern standard and means we do not have to do complex pointer calculations to translate addresses based on what segment we are in. However, this means we need to implement paging to track what section we are trying to access to prevent accesses across segments.

