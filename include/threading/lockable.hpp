#ifndef LOCKABLE
#define LOCKABLE

#include <mutex>

namespace BlueBear {
  namespace Threading {

    template < typename T > class Lockable {
      T object;
      std::mutex mutex;

      Lockable( T object ) : object( object ) {}
    };

  }
}


#endif
