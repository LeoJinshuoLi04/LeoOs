#include <stddef.h>

class string {
  public: 
    static const size_t MAX_SIZE = 128;

    string();
    string(const char* str);
    string(const string& other);
    bool operator==(const string& other) const;

    void push_back(char c);
    void pop_back();
    size_t size() const;
  private:
    char mem[MAX_SIZE];
    size_t length;
};