#include "models/walljoint.hpp"
#include "tools/utility.hpp"
#include <glm/glm.hpp>
#include <algorithm>

namespace BlueBear::Models {

  using CardinalDirection = Tools::CardinalDirection;
  using Directional = Tools::Directional;

  void WallJoint::setByCardinalDirection( const CardinalDirection& direction ) {
    switch ( direction ) {
      case CardinalDirection::North:
        north = true;
        return;

      case CardinalDirection::South:
        south = true;
        return;

      case CardinalDirection::East:
        east = true;
        return;

      case CardinalDirection::West:
        west = true;
        return;

      case CardinalDirection::Northeast:
        northeast = true;
        return;

      case CardinalDirection::Southeast:
        southeast = true;
        return;

      case CardinalDirection::Southwest:
        southwest = true;
        return;

      case CardinalDirection::Northwest:
        northwest = true;
        return;
    }
  }

  bool WallJoint::isElbow() const {
    return ( north || south ) && ( east || west );
  }

  std::vector< std::vector< WallJoint > > WallJoint::createFromList( unsigned int x, unsigned int y, const std::vector< WallSegment >& array ) {
    std::vector< std::vector< WallJoint > > result;
    result.resize( y );
    for( auto& subarray : result ) {
      subarray.resize( x );
    }

    for( const WallSegment& segment : array ) {
      Directional directional = Tools::getDirectional( segment.start, segment.end );

      glm::ivec2 cursor = directional.start;
      int originalDistance = Tools::Utility::distance( directional.start, directional.end );
      for( int distance = originalDistance; distance >= 0; distance-- ) {
        if( distance == originalDistance ) {
          // First one - use directional.cardinal
          result[ cursor.y ][ cursor.x ].setByCardinalDirection( directional.cardinal );
        } else if( distance == 0 ) {
          // Last one - use getOpposingDirection( directional.cardinal )
          result[ cursor.y ][ cursor.x ].setByCardinalDirection( Tools::getOpposingDirection( directional.cardinal ) );
        } else {
          // Everything in between - apply both directional.cardinal and getOpposingDirection( directional.cardinal )
          result[ cursor.y ][ cursor.x ].setByCardinalDirection( directional.cardinal );
          result[ cursor.y ][ cursor.x ].setByCardinalDirection( Tools::getOpposingDirection( directional.cardinal ) );
        }

        cursor += directional.direction;
      }
    }

    return result;
  }

}
