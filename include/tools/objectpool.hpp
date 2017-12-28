#ifndef BB_OBJECT_POOL
#define BB_OBJECT_POOL

#include <tbb/concurrent_queue.h>
#include <functional>
#include <memory>

namespace BlueBear {
  namespace Tools {

    template< typename Class >
    class ObjectPool {
      tbb::concurrent_queue< std::unique_ptr< Class > > pool;
      std::function< std::unique_ptr< Class >() > createMethod;

    public:
      ObjectPool( std::function< std::unique_ptr< Class >() > createMethod ) : createMethod( createMethod ) {}

      void acquire( std::function< void( Class& ) > predicate ) {
        // Acquire or create resource
        std::unique_ptr< Class > resource;
        if( !pool.try_pop( resource ) ) {
          resource = createMethod();
        }

        // Use resource
        predicate( *resource );

        // Replace resource
        pool.push( std::move( resource ) );
      }
    };

  }
}

#endif
