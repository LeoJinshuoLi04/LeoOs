#include "scheduler.hpp"
#include "HeapAllocator.hpp"
#include "terminal.hpp"

extern Terminal* global_terminal;

volatile int taskCounter{0};

void idleFunction(){
  global_terminal->write("Idle task running...\n");
  while(true){
    if(!taskCounter) global_terminal->write("Idle task running...\n");
    taskCounter = 1;
    while(taskCounter);
  }
}

void idleFunction2() {
  global_terminal->write("Idle task 2 running...\n");
  while(true){
    if(taskCounter) global_terminal->write("Idle task 2 running...\n");
    taskCounter = 0;
    while(!taskCounter);
  }
}

void RoundRobinScheduler::init(){
  createTask(reinterpret_cast<uint32_t>(&idleFunction));
  createTask(reinterpret_cast<uint32_t>(&idleFunction2));
};

ProcessControlBlock* RoundRobinScheduler::createTask(uint32_t functionAddress) {
    static uint32_t processId = 0;
    using ProcessState = ProcessControlBlock::ProcessState;

    auto pcb = reinterpret_cast<ProcessControlBlock*>(heapAllocator.lalloc(sizeof(ProcessControlBlock)));
    uint32_t stackBase = reinterpret_cast<uint32_t>(heapAllocator.lalloc(PMM_PAGE_SIZE));
  
    uint32_t* stackPointer = reinterpret_cast<uint32_t*>(stackBase + PMM_PAGE_SIZE);


    // These are the values the 'iret' instruction will pop
    *(--stackPointer) = 0x202;            // EFLAGS: Bit 9 is IF (Interrupt Flag). Enables interrupts in the task.
    *(--stackPointer) = 0x08;             // CS: Your Kernel Code Segment (check your GDT)
    *(--stackPointer) = functionAddress;  // EIP: Where the CPU starts executing

    // These are the values your 'popal' (pop all) instruction will pop
    // We push 8 dummy values for EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
    for (int i = 0; i < 8; ++i ) {
        *(--stackPointer) = 0;
    }

    // These are the values your segment 'pop' instructions (ds, es, etc.) will pop
    // We usually use 0x10 for the Kernel Data Segment
    for (int i = 0; i < 4; ++i) {
        *(--stackPointer) = 0x10;
    }

    pcb->stackPointer = reinterpret_cast<uint32_t>(stackPointer);
    pcb->stackAddress = stackBase;
    pcb->state = ProcessState::ready;
    pcb->processId = ++processId;

    addTask(pcb);
    return pcb;
}

void RoundRobinScheduler::addTask(ProcessControlBlock* pcb){
  if(!readyQueue){
    if(!currentProcess) currentProcess = pcb;
    else readyQueue = pcb;
  }else{
    auto current = readyQueue;
    while(current->next){
      current = current->next;
    }
    current->next = pcb;
  }
}

void RoundRobinScheduler::killTask(const uint32_t processId){
  auto current = readyQueue;
  if(current->processId == processId){
    readyQueue = current->next;
    freeTaskMemory(current);
    return;
  }
  while(current->next){
    if(current->next->processId == processId){
      current->next = current->next->next;
      freeTaskMemory(current->next);
      return;
    }
    current = current->next;
  }
};

void RoundRobinScheduler::freeTaskMemory(ProcessControlBlock* pcb){
  heapAllocator.lfree(reinterpret_cast<void*>(pcb->stackAddress));
  heapAllocator.lfree(pcb);
}

ProcessControlBlock* RoundRobinScheduler::scheduleNextTask(){
  global_terminal->write("Scheduling next task...\n");
  if(!readyQueue) return currentProcess;
  global_terminal->write("Switching from process ");
  global_terminal->write_dec(currentProcess->processId);
  auto current = readyQueue;
  while(current->next){
    current = current->next;
  }
  current->next = currentProcess;
  global_terminal->write(" to process ");
  global_terminal->write_dec(readyQueue->processId);
  currentProcess = readyQueue;
  readyQueue = readyQueue->next;
  currentProcess->next = nullptr;
  return currentProcess;
};
  
RoundRobinScheduler scheduler;