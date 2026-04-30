Interrupt Service Routines (ISR): Assembly stubs serve as low-level bridges, preserving CPU state and registers before delegating execution to high-level C++ logic.

Once the idt jumps to the kernel and saves the current state, it will then call the isr to actually run whatever is needed to be done when a certain interrupt occurs. It will then return control back to the kernel once it is done to reload the registers and continue program running. This is done because only in assembly can we set the registers and call irt (interrupt return);
