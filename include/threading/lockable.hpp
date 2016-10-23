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
      // Default constructor needed for STL containers; using this is discouraged unless you need a default object
      // Instead, use the static "create" method to create a Lockable
      Lockable() :
        object( std::shared_ptr< T >() ),
        mutex( std::shared_ptr< std::mutex >() ) {}

      /**
       * Use this method instead to create Lockable objects
       * This is to work around ambiguity which would result in the above constructor being called for empty argument lists
       * I'm really counting on RVO here
       */
      template< typename... Args > static Lockable create( Args&&... args ) {
        Lockable< T > lockable;

        lockable.set( args... );

        return lockable;
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

      /**
       * Run an unsafe operation (call the function on the wrapped object without locking)
       * You should only need this in very exceptional scenarios, which is why there is no syntactic sugar function for "unsafe"
       */
      template < typename R > R unsafe( std::function< R( T& ) > predicate ) {
        return predicate( *object );
      }

      explicit operator bool() const {
        return object.operator bool();
      }
    };

  }
}


#endif
