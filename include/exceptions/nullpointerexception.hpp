#ifndef NULLPOINTEREXCEPTION
#define NULLPOINTEREXCEPTION

#include "log.hpp"
#include <exception>

namespace BlueBear {
  namespace Exceptions {

    struct NullPointerException : public std::exception {

      const char* what() const throw() {
        return "Required pointer is null!";
      }

      template< typename PointerType >
      static inline void check( const std::string& originModule, PointerType* pointer ) {
        if( !pointer ) {
          Log::getInstance().error( originModule, "Null pointer assertion failed" );
          throw NullPointerException();
        }
      }

    };

  }
}

#endif
