#include "src/arch/i386/ports.hpp"
#include "scheduler.hpp"
#include "terminal.hpp"

extern Terminal* global_terminal;

void timer_init(uint32_t hz) {
    uint32_t divisor = 1193182 / hz;

    // Send the command byte (0x36): 
    // Channel 0, Access lo/hi byte, Square Wave Mode, 16-bit binary
    outb(0x43, 0x36);

    // Send the divisor (split into two bytes)
    outb(0x40, (uint8_t)(divisor & 0xFF));        // Low byte
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF)); // High byte
}

extern "C" uint32_t pitHandler(uint32_t current_esp) {
    static int tickCount{0};
    if(++tickCount % 100 == 0){
        tickCount = 0;
        global_terminal->write("Timer Interrupt: Scheduling next task...\n");
        scheduler.currentProcess->stackPointer = current_esp;
        return scheduler.scheduleNextTask()->stackPointer;
    }
    return current_esp;
}
