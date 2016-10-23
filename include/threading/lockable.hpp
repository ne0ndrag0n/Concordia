#ifndef LOCKABLE
#define LOCKABLE

#include <mutex>
#include <memory>
#include <functional>

namespace BlueBear {
  namespace Threading {

    template < typename T > class Lockable {
    private:
      const std::shared_ptr< T > object;
      const std::shared_ptr< std::mutex > mutex;

    public:
      template < typename... Args > Lockable( bool nullPointer, Args&&... args ) :
        object( nullPointer ? std::shared_ptr< T >() : std::make_shared< T >( args... ) ),
        mutex( std::make_shared< std::mutex >() ) {}

      // Synctactic sugar for void methods that avoids the generic qualifier
      void lock( std::function< void( T& ) > predicate ) {
        lock<void>( predicate );
      }

      template < typename R > R lock( std::function< R( T& ) > predicate ) {
        std::unique_lock< std::mutex > lock( *mutex );
        return predicate( *object );
      }
    };

  }
}


#endif
