#include "scripting/floormap.hpp"

namespace BlueBear {
  namespace Scripting {

    FloorMap::FloorMap( unsigned int levels, unsigned int x, unsigned int y ) : levels( levels ), dimensionX( x ), dimensionY( y ) {
      // Preallocate/reserve the items
      tiles.resize( levels * x * y );
    }

    // Compute the location in this 1D array, then get the tile
    std::shared_ptr< Tile > FloorMap::getTile( unsigned int level, unsigned int x, unsigned int y ) {

    }

    // Get a position from the 3D coordinates
    unsigned int FloorMap::getSingleIndex( unsigned int level, unsigned int x, unsigned int y ) {
      unsigned int origin = level * ( dimensionX * dimensionY );

      // check this formula
      return origin + ( ( dimensionX * y ) + x );
    }

  }
}
