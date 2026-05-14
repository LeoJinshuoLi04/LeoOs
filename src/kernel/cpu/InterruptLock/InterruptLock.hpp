#pragma once

class InterruptLock{
  InterruptLock(){
    asm volatile("cli");
  };
  ~InterruptLock(){
    asm volatile("sti");
  };
};