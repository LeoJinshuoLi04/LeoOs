#pragma once
#include "string.hpp"

// Everything matches this signature
typedef void (*command_func_t)(int argc, char** argv);

struct ShellCommand {
    const char* name;
    const char* description;
    command_func_t handler;
    int min_args;
};


class Shell {
  public:
    Shell();
    static void execute(string buffer);
  private:
    static ShellCommand commands[];
    static const int num_commands;
};