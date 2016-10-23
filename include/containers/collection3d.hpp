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

        virtual T& getItemDirectByRef( unsigned int direct ) {
          return getItems()[ direct ];
        }
        T getItemDirect( unsigned int direct ) {
          return getItemDirectByRef( direct );
        }

        T& getItemByRef( unsigned int level, unsigned int x, unsigned int y ) {
          return getItemDirectByRef( getSingleIndex( level, x, y ) );
        }
        T getItem( unsigned int level, unsigned int x, unsigned int y ) {
          return getItemByRef( level, x, y );
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
    };

  }
}


#endif
