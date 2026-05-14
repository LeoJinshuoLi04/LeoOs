#include "scheduler.hpp"
#include "HeapAllocator.hpp"
#include "terminal.hpp"

extern Terminal* global_terminal;

volatile int taskCounter = 0; // Must be volatile!

void idleFunction (){
  while (true) 
    asm volatile("hlt");
};

void FunctionA() {
  while(true) {
    global_terminal->write("A ");
    for(volatile int i = 0; i < 10000000; i++);
  }
}

void FunctionB() {
  while(true) {
    global_terminal->write("B ");
    for(volatile int i = 0; i < 10000000; i++);
  }
}

void RoundRobinScheduler::init(){
  using namespace memory;
  auto kernelTaskPCB = construct<ProcessControlBlock>();
  kernelTaskPCB->processId = 0;
  kernelTaskPCB->state = ProcessControlBlock::ProcessState::running;
  currentProcess = kernelTaskPCB;
  idleProcess = createTask(reinterpret_cast<uint32_t>(&idleFunction));

  createTask(reinterpret_cast<uint32_t>(&FunctionA));
  createTask(reinterpret_cast<uint32_t>(&FunctionB));
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
    global_terminal->write("Adding process ");
    readyQueue = pcb;
    global_terminal->write_dec(currentProcess->processId);
    global_terminal->write("to ready queue");
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
  global_terminal->write("Switching from process ");
  global_terminal->write_dec(currentProcess->processId);
  // append to readyQueue;
  if(currentProcess->processId > 1)[[likely]]{
    if(!readyQueue){
      readyQueue = currentProcess;
      readyQueue->next = nullptr;
    } else {
      auto current = readyQueue;
      while(current->next){
        current = current->next;
      }
      current->next = currentProcess;
    }
  }else if(currentProcess->processId == 0){
    heapAllocator.lfree(currentProcess);
  }

  //Get next task from queue or idleTask;
  if(!readyQueue){
    currentProcess = idleProcess;
  } else {
    currentProcess = readyQueue;
    readyQueue = readyQueue->next;
    currentProcess->next = nullptr;
  }
  global_terminal->write(" to process ");
  global_terminal->write_dec(currentProcess->processId);
  global_terminal->write("\n");
  return currentProcess;
};
  
RoundRobinScheduler scheduler;