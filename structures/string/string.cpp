#include "string.hpp"

string::string() : length(0){}

string::string(const char* str) : length(0) {
  while(length < MAX_SIZE -1 && str[length] != '\0'){
    push_back(str[length++]);
  }
  mem[length] = '\0';
}

string::string(const string& other) : length(other.length) {
  for(size_t i = 0; i< length; ++i){
    mem[i] = other.mem[i];
  }
};

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

size_t string::size() const {
  return length;
};