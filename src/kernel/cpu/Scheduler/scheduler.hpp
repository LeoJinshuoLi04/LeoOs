#pragma once

#include "processControlBlock.hpp"

class RoundRobinScheduler {
  public:
    ProcessControlBlock* currentProcess = nullptr;
    ProcessControlBlock* readyQueue = nullptr;
    ProcessControlBlock* idleProcess = nullptr;
    RoundRobinScheduler(){};
    void init();
    ProcessControlBlock* createTask(const uint32_t functionAddress);
    ProcessControlBlock* scheduleNextTask();
    void killTask(const uint32_t processId);
  private:
    void addTask(ProcessControlBlock* pcb);
    void freeTaskMemory(ProcessControlBlock* pcb);
};

extern RoundRobinScheduler scheduler;