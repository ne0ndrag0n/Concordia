#ifndef BB_GENEXC
#define BB_GENEXC

#include <exception>
#define EXCEPTION_TYPE( Type, message ) struct Type : public std::exception { \
    const char* what() const throw() { \
      return message; \
    } \
  };

#endif
