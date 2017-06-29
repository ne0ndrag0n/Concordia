#ifndef NULLPOINTEREXCEPTION
#define NULLPOINTEREXCEPTION

#include <exception>

namespace BlueBear {
  namespace Exceptions {

    struct NullPointerException : public std::exception {

      const char* what() const throw() {
        return "Required pointer is null!";
      }

    };

  }
}

#endif
