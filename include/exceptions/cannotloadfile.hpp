#ifndef CANNOTLOADFILEEXCEPTION
#define CANNOTLOADFILEEXCEPTION

#include <exception>

namespace BlueBear {
  namespace Exceptions {

    struct CannotLoadFileException : public std::exception {

      const char* what() const throw() {
        return "Could not load a required file!";
      }

    };

  }
}

#endif
