#ifndef COLLECTION3D
#define COLLECTION3D

#include <vector>
#include <memory>
#include <utility>
#include <functional>

namespace BlueBear {
  namespace Containers {

    template <typename T> class Collection3D {

      protected:
        std::vector< T > items;
        unsigned int getSingleIndex( unsigned int level, unsigned int x, unsigned int y ) {
          unsigned int origin = level * ( dimensionX * dimensionY );

          // check this formula
          return origin + ( ( dimensionX * y ) + x );
        }

        std::vector< T >& getItems() {
          return items;
        }

      public:
        using Predicate = std::function< void( T& ) >;

        unsigned int dimensionX;
        unsigned int dimensionY;
        unsigned int levels;
        Collection3D( unsigned int levels, unsigned int x, unsigned int y ) : levels( levels ), dimensionX( x ), dimensionY( y ) {}

        // fuckin' const-correctness, how does it work
        T getItem( unsigned int level, unsigned int x, unsigned int y ) {
          return getItems()[ getSingleIndex( level, x, y ) ];
        }
        T& getItemByRef( unsigned int level, unsigned int x, unsigned int y ) {
          return getItems()[ getSingleIndex( level, x, y ) ];
        }
        T getItemDirect( unsigned int direct ) {
          return getItems()[ direct ];
        }
        T& getItemDirectByRef( unsigned int direct ) {
          return getItems()[ direct ];
        }
        virtual unsigned int getLength() {
          return getItems().size();
        }
        //void setItem( unsigned int level, unsigned int x, unsigned int y, T item );
        virtual void pushDirect( T item ) {
          getItems().push_back( item );
        }

        virtual void moveDirect( T item ) {
          getItems().push_back( std::move( item ) );
        }

        // little shit functions for functional programming
        virtual void operationOn( unsigned int direct, Predicate func ) {
          func( getItems()[ direct ] );
        }
    };

  }
}


#endif
