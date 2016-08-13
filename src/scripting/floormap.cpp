#include "scripting/floormap.hpp"
#include "log.hpp"
#include <string>

namespace BlueBear {
  namespace Scripting {

    FloorMap::FloorMap( unsigned int levels, unsigned int x, unsigned int y ) : levels( levels ), dimensionX( x ), dimensionY( y ) {}

    // Compute the location in this 1D array, then get the tile
    std::shared_ptr< Tile > FloorMap::getTile( unsigned int level, unsigned int x, unsigned int y ) {
      return tiles[ getSingleIndex( level, x, y ) ];
    }

    // Get a position from the 3D coordinates
    unsigned int FloorMap::getSingleIndex( unsigned int level, unsigned int x, unsigned int y ) {
      unsigned int origin = level * ( dimensionX * dimensionY );

      // check this formula
      return origin + ( ( dimensionX * y ) + x );
    }

    void FloorMap::pushDirect( std::shared_ptr< Tile > tile ) {
      tiles.push_back( tile );
    }

  }
}
