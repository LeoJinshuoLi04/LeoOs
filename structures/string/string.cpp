#include "string.hpp"

string::string() : length(0){}

string::string(const char* str) {
    size_t i = 0;
    while (str[i] != '\0' && i < MAX_SIZE - 1) {
        mem[i] = str[i];
        i++;
    }
    length = i;
    mem[length] = '\0';
}

string::string(const string& other) : length(other.length) {
  for(size_t i = 0; i< length; ++i){
    mem[i] = other.mem[i];
  }
  mem[length] = '\0';
};

string& string::operator=(const string& other) {
    if (this != &other) { // Protect against self-assignment
        length = other.length;
        for (size_t i = 0; i < length; ++i) {
            mem[i] = other.mem[i];
        }
        mem[length] = '\0';
    }
    return *this;
}

bool string::operator==(const string& other) const {
  if(length != other.length) return false;
  for(size_t i = 0; i< length; i++){
    if(mem[i] != other.mem[i]) return false;
  }
  return true;
};

void string::push_back(char c){
  if(length < MAX_SIZE - 1){
    mem[length] = c;
    mem[++length] = '\0';
  }
};

void string::pop_back(){
  if(length > 0){
    mem[--length] = '\0';
  }
};

char* string::cstr() {
  return &mem[0];
}

const char* string::cstr() const{
  return mem;
};

size_t string::size() const {
  return length;
};

int string::tokenize(char* buf, char** argv, int max_args) {
    int argc = 0;
    bool in_token = false;

    for (char* p = buf; *p != '\0'; p++) {
        if (*p == ' ') {
            *p = '\0';
            in_token = false;
        } else {
            if (!in_token) {
                if (argc < max_args) {
                    argv[argc] = p;
                    argc++;
                }
                in_token = true;
            }
        }
    }
    return argc;
}
