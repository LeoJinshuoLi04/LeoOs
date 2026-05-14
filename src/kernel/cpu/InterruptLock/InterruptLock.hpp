#pragma once

class InterruptLock{
  InterruptLock(){
    asm volatile("cli");
  };
  ~InterruptLock(){
    asm volatile("sti");
  };
  InterruptLock(const InterruptLock&) = delete;
  InterruptLock& operator=(const InterruptLock&) = delete;
};