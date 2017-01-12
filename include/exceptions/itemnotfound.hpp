#ifndef ITEMNOTFOUNDEXCEPTION
#define ITEMNOTFOUNDEXCEPTION

#include <exception>

namespace BlueBear {
  namespace Exceptions {

    struct ItemNotFoundException : public std::exception {

      const char* what() const throw() {
        return "Required item not found!";
      }

    };

  }
}

#endif
