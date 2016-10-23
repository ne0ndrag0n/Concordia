#ifndef LOCKABLE
#define LOCKABLE

#include <mutex>
#include <memory>
#include <functional>

namespace BlueBear {
  namespace Threading {

    template < typename T > class Lockable {
    private:
      std::shared_ptr< T > object;
      std::shared_ptr< std::mutex > mutex;

    public:
      Lockable() :
        object( std::shared_ptr< T >() ),
        mutex( std::shared_ptr< std::mutex >() ) {}

      template < typename... Args > Lockable( bool nullPointer, Args&&... args ) :
        mutex( std::make_shared< std::mutex >() ) {

        if( nullPointer ) {
          object = std::shared_ptr< T >();
        } else {
          object = std::make_shared< T >( args... );
        }
      }

      template< typename... Args > void set( Args&&... args ) {
        object = std::make_shared< T >( args... );
        mutex = std::make_shared< std::mutex >();
      }

      void reset() {
        object = std::shared_ptr< T >();
        mutex = std::shared_ptr< std::mutex >();
      }

      // Synctactic sugar for void methods that avoids the generic qualifier
      void lock( std::function< void( T& ) > predicate ) {
        lock<void>( predicate );
      }

      template < typename R > R lock( std::function< R( T& ) > predicate ) {
        std::unique_lock< std::mutex > lock( *mutex );
        return predicate( *object );
      }

      explicit operator bool() const {
        return object.operator bool();
      }
    };

  }
}


#endif
