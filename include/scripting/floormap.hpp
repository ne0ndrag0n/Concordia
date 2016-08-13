#ifndef FLOORMAP
#define FLOORMAP

#include <vector>
#include <memory>

namespace BlueBear {
  namespace Scripting {
    class Tile;

    class FloorMap {

      private:
        unsigned int dimensionX;
        unsigned int dimensionY;
        unsigned int levels;
        std::vector< std::shared_ptr< Tile > > tiles;
        unsigned int getSingleIndex( unsigned int level, unsigned int x, unsigned int y );

      public:
        FloorMap( unsigned int levels, unsigned int x, unsigned int y );
        std::shared_ptr< Tile > getTile( unsigned int level, unsigned int x, unsigned int y );
        void setTile( unsigned int level, unsigned int x, unsigned int y, std::shared_ptr< Tile > tile );
        void pushDirect( std::shared_ptr< Tile > tile );
    };

  }
}

#endif
