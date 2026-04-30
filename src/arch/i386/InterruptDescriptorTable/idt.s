.global idt_load
idt_load:
    mov 4(%esp), %edx
    lidt (%edx)
    ret
