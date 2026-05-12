#include "scheduler.hpp"
#include "HeapAllocator.hpp"

void idleFunction(){
  while(1);
}

void RoundRobinScheduler::init(){
  createTask(reinterpret_cast<uint32_t>(&idleFunction));
};

ProcessControlBlock* RoundRobinScheduler::createTask(uint32_t functionAddress){
  static uint32_t processId = 0;
  using ProcessState = ProcessControlBlock::ProcessState;

  auto pcb = reinterpret_cast<ProcessControlBlock*>(heapAllocator.lalloc(sizeof(ProcessControlBlock)));
  auto taskStackAddress = reinterpret_cast<uint32_t>(heapAllocator.lalloc(PMM_PAGE_SIZE));

  pcb->stackPointer = taskStackAddress;
  pcb->stackAddress = taskStackAddress;
  pcb->state = ProcessState::ready;
  pcb->processId = ++processId;

  addTask(pcb);
  return pcb;
};

void RoundRobinScheduler::addTask(ProcessControlBlock* pcb){
  if(!readyQueue){
    readyQueue = pcb;
    pcb->next = nullptr;
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
  if(!readyQueue) return currentProcess;
  auto current = readyQueue;
  while(current->next){
    current = current->next;
  }
  current->next = currentProcess;
  currentProcess = readyQueue;
  readyQueue = readyQueue->next;
  return currentProcess;
};
  
RoundRobinScheduler scheduler;