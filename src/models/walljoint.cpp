#include "models/walljoint.hpp"
#include "tools/utility.hpp"
#include <glm/glm.hpp>
#include <algorithm>

namespace BlueBear::Models {

  using CardinalDirection = Tools::CardinalDirection;

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

  bool WallJoint::isSingleAxis() const {
    return ( ( north || south ) && !( east || west ) ) ||
           ( ( east || west ) && !( north || south ) );
  }

  bool WallJoint::isElbow() const {
    return ( north || south ) && ( east || west );
  }

  bool WallJoint::isCross() const {
    return
      ( ( north && south ) && ( east && !west ) ) ||
      ( ( north && south ) && ( !east && west ) ) ||
      ( ( east && west ) && ( north && !south ) ) ||
      ( ( east && west ) && ( !north && south ) );
  }

  bool WallJoint::isFull() const {
    return north && south && east && west;
  }

  struct Directional {
    glm::ivec2 remaining;
    glm::ivec2 direction;
    glm::ivec2 start;
    glm::ivec2 end;
    CardinalDirection cardinal;
  };

  CardinalDirection getCardinalDirection( const glm::ivec2& direction ) {

    if( direction.x == 0 && direction.y < 0 ) {
      return CardinalDirection::North;
    }

    if( direction.x == 0 && direction.y > 0 ) {
      return CardinalDirection::South;
    }

    if( direction.x > 0 && direction.y == 0 ) {
      return CardinalDirection::East;
    }

    if( direction.x < 0 && direction.y == 0 ) {
      return CardinalDirection::West;
    }

    if( direction.x > 0 && direction.y < 0 ) {
      return CardinalDirection::Northeast;
    }

    if( direction.x > 0 && direction.y > 0 ) {
      return CardinalDirection::Southeast;
    }

    if( direction.x < 0 && direction.y > 0 ) {
      return CardinalDirection::Southwest;
    }

    if( direction.x < 0 && direction.y < 0 ) {
      return CardinalDirection::Northwest;
    }

  }

  Directional getDirectional( const glm::ivec2& start, const glm::ivec2& end ) {
    glm::ivec2 direction = Tools::Utility::normalize( end - start );
    CardinalDirection cardinal = getCardinalDirection( direction );

    if( cardinal == CardinalDirection::West ) {
      return { start - end, { 1, 0 }, end, start, CardinalDirection::East };
    }

    if( cardinal == CardinalDirection::South ) {
      return { start - end, { 0, -1 }, end, start, CardinalDirection::North };
    }

    if( cardinal == CardinalDirection::Northwest ) {
      return { start - end, { 1, 1 }, end, start, CardinalDirection::Southeast };
    }

    if( cardinal == CardinalDirection::Southwest ) {
      return { start - end, { 1, -1 }, end, start, CardinalDirection::Northeast };
    }

    return { end - start, direction, start, end, cardinal };
  }

  std::vector< std::vector< WallJoint > > WallJoint::createFromList( unsigned int x, unsigned int y, const Json::Value& array ) {
    if ( !array.isArray() ) {
      throw InvalidTypeException();
    }

    std::vector< std::vector< WallJoint > > result;
    result.resize( y );
    for( auto& subarray : result ) {
      subarray.resize( x );
    }

    for( const Json::Value& wallDescriptor : array ) {
      Directional directional = getDirectional(
        { wallDescriptor[ "start" ][ 0 ].asInt(), wallDescriptor[ "start" ][ 1 ].asInt() },
        { wallDescriptor[ "end" ][ 0 ].asInt(), wallDescriptor[ "end" ][ 1 ].asInt() }
      );

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
