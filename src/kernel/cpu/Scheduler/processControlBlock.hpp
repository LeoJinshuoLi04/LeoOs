#include <stdint.h>

struct ProcessControlBlock {
  enum class ProcessState {
    ready,
    running,
    waiting,
    blocked
  };

  uint32_t stackPointer;
  uint32_t stackAddress;
  uint32_t processId;  
  ProcessState state;  
  ProcessControlBlock* next = nullptr;
};