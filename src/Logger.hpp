#ifndef Logger_HPP
#define Logger_HPP

#include <cstdlib>
#include <iostream>
#include <sstream>

class ErrorLogMessage : public std::basic_ostringstream<char> {
  public:
    ~ErrorLogMessage() {
        std::cerr << "Fatal error: " << str().c_str();
        exit(EXIT_FAILURE);
    }
};

#define DIE ErrorLogMessage()

#endif
