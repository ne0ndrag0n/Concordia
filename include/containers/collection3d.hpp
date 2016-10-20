#ifndef COLLECTION3D
#define COLLECTION3D

#include <vector>
#include <memory>
#include <utility>

namespace BlueBear {
  namespace Containers {

    template <typename T> class Collection3D {

      private:
        std::vector< T > items;
        unsigned int getSingleIndex( unsigned int level, unsigned int x, unsigned int y ) {
          unsigned int origin = level * ( dimensionX * dimensionY );

          // check this formula
          return origin + ( ( dimensionX * y ) + x );
        }

      public:
        unsigned int dimensionX;
        unsigned int dimensionY;
        unsigned int levels;
        Collection3D( unsigned int levels, unsigned int x, unsigned int y ) : levels( levels ), dimensionX( x ), dimensionY( y ) {}
        T getItem( unsigned int level, unsigned int x, unsigned int y ) {
          return items[ getSingleIndex( level, x, y ) ];
        }
        T& getItemByRef( unsigned int level, unsigned int x, unsigned int y ) {
          return getItem( level, x, y );
        }
        T getItemDirect( unsigned int direct ) {
          return items[ direct ];
        }
        T& getItemDirectByRef( unsigned int direct ) {
          return getItemDirect( direct );
        }
        unsigned int getLength() {
          return items.size();
        }
        //void setItem( unsigned int level, unsigned int x, unsigned int y, T item );
        void pushDirect( T item ) {
          items.push_back( item );
        }

        void moveDirect( T item ) {
          items.push_back( std::move( item ) );
        }
    };

  }
}


#endif
