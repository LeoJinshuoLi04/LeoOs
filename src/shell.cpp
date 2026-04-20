#include "shell.hpp"
#include "terminal.hpp"

extern Terminal* global_terminal;

// Forward declarations of your command functions
void cmd_help(int argc, char** argv);
void cmd_clear(int argc, char** argv);

// Actual definition and initialization of the static array
ShellCommand Shell::commands[] = {
    {"help",  "Show this menu",  cmd_help,  0},
    {"clear", "Clear the screen", cmd_clear, 0}
};

// Calculate the number of commands automatically
const int Shell::num_commands = sizeof(Shell::commands) / sizeof(ShellCommand);

Shell::Shell() {}

void Shell::execute(string buffer) {
    char* argv[100];
    int argc = buffer.tokenize(buffer.cstr(), argv, 100);
    if(argc > 0){
      string command = string(argv[0]);
      for(int i = 0; i< num_commands; i++){
        if(command == string(commands[i].name)){
          if(argc - 1 < commands[i].min_args) { // -1 because argv[0] is the command itself
              global_terminal->write("Error: '");
              global_terminal->write(commands[i].name);
              global_terminal->write("' requires ");
              global_terminal->write_dec(commands[i].min_args);
              global_terminal->write(" arguments, but ");
              global_terminal->write_dec(argc - 1);
              global_terminal->write(" were provided.\n");
          }else{
            commands[i].handler(argc, argv);
          }
          global_terminal->write(">> ");
          return;
        }
      }
      global_terminal->write(buffer.cstr());
      global_terminal->write("\n");
    }
    global_terminal->write(">> ");
}

// Implement your command logic below
void cmd_help(int argc, char** argv) {
    (void)argc; (void)argv;
    global_terminal->write("LeoOS Help System\n");
}

void cmd_clear(int argc, char** argv){
  (void)argc; (void)argv;
  global_terminal->clear();
}