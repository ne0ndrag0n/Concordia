#ifndef REUSABLE_OBJECT_VECTOR
#define REUSABLE_OBJECT_VECTOR

#include <vector>
#include <optional>
#include <functional>

namespace BlueBear::Containers {

  template< typename T > class ReusableObjectVector {
    std::vector< std::optional< T > > vector;

  public:
    int insert( T object ) {
      for( int i = 0; i != vector.size(); i++ ) {
        if( !vector[ i ] ) {
          vector[ i ] = object;
          return i;
        }
      }

      vector.emplace_back( object );
      return vector.size() - 1;
    };

    void remove( int index ) {
      vector[ index ].reset();
    };

    void remove( T object ) {
      for( int i = 0; i != vector.size(); i++ ) {
        if( vector[ i ] && *vector[ i ] == object ) {
          return remove( i );
        }
      }
    };

    T get( int index ) {
      return *vector[ index ];
    };

    void each( std::function< void( T& ) > predicate ) {
      for( int i = 0; i != vector.size(); i++ ) {
        if( vector[ i ] ) {
          predicate( *vector[ i ] );
        }
      }
    };

    void each( std::function< void( std::optional< T >& ) > predicate ) {
      for( int i = 0; i != vector.size(); i++ ) {
        predicate( vector[ i ] );
      }
    };
  };

}

#endif
