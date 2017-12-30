#ifndef BB_OBJECT_POOL
#define BB_OBJECT_POOL

#include <tbb/concurrent_queue.h>
#include <functional>
#include <memory>
#include <mutex>

namespace BlueBear {
  namespace Tools {

    template< typename Class >
    class ObjectPool {
      tbb::concurrent_queue< std::unique_ptr< Class > > pool;
      std::function< std::unique_ptr< Class >() > createMethod;

    public:
      ObjectPool( std::function< std::unique_ptr< Class >() > createMethod ) : createMethod( createMethod ) {}

      void acquire( std::function< void( Class& ) > predicate ) {
        static std::mutex mutex;

        // Acquire or create resource
        std::unique_ptr< Class > resource;
        bool obtained;
        {
          std::lock_guard< std::mutex > lock( mutex );
          obtained = pool.try_pop( resource );
        }
        if( !obtained ) {
          resource = createMethod();
        }

        // Use resource
        predicate( *resource );

        // Replace resource
        {
          std::lock_guard< std::mutex > lock( mutex );
          pool.push( std::move( resource ) );
        }
      }
    };

  }
}

#endif
