#ifndef CONCCOLLECTION3D
#define CONCCOLLECTION3D

#include "containers/collection3d.hpp"
#include "threading/lockable.hpp"
#include <mutex>
#include <vector>
#include <memory>
#include <utility>
#include <functional>

namespace BlueBear {
  namespace Containers {

    /**
     * A ConcCollection3D protects both its own collection, and the objects within.
     */
    template < typename T > class ConcCollection3D : private Collection3D< Threading::Lockable< T > > {

    private:
      std::mutex vectorMutex;

    public:
      // Expose these via superclass call + lock
      unsigned int getLength() {
        std::unique_lock< std::mutex > lock( vectorMutex );
        return Collection3D< Threading::Lockable< T > >::getLength();
      }

      void pushDirect( T item ) {
        std::unique_lock< std::mutex > lock( vectorMutex );

        Collection3D< Threading::Lockable< T > >::pushDirect( Threading::Lockable< T >( item ) );
      }

      void moveDirect( T item ) {
        std::unique_lock< std::mutex > lock( vectorMutex );

        Collection3D< Threading::Lockable< T > >::getItems().push_back( std::move( Threading::Lockable< T >( item ) ) );
      }

      // Everything you do with any object stored in a ConcCollection3D is gatekept by these functional methods
      // You must pass a lambda to do anything!


    };
  }
}

#endif
