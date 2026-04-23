#include <stddef.h>

class string {
  public: 
    static const size_t MAX_SIZE = 128;

    string();
    string(const char* str);
    string(const string& other);
    string& operator=(const string& other);
    bool operator==(const string& other) const;
    char& operator[](int index);

    void push_back(char c);
    void pop_back();
    size_t size() const;
    char* cstr();
    const char* cstr() const;

    int tokenize(char* buf, char** argv_out, int max_args);
  private:
    char mem[MAX_SIZE];
    size_t length;
};