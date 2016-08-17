#ifndef COLLECTION3D
#define COLLECTION3D

#include <vector>
#include <memory>

namespace BlueBear {
  namespace Containers {

    template <typename T> class Collection3D {

      private:
        unsigned int dimensionX;
        unsigned int dimensionY;
        unsigned int levels;
        std::vector< T > items;
        unsigned int getSingleIndex( unsigned int level, unsigned int x, unsigned int y ) {
          unsigned int origin = level * ( dimensionX * dimensionY );

          // check this formula
          return origin + ( ( dimensionX * y ) + x );
        }

      public:
        Collection3D( unsigned int levels, unsigned int x, unsigned int y ) : levels( levels ), dimensionX( x ), dimensionY( y ) {}
        T getItem( unsigned int level, unsigned int x, unsigned int y ) {
          return items[ getSingleIndex( level, x, y ) ];
        }
        //void setItem( unsigned int level, unsigned int x, unsigned int y, T item );
        void pushDirect( T item ) {
          items.push_back( item );
        }
    };

  }
}


#endif
