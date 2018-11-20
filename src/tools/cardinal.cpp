#include "tools/cardinal.hpp"

namespace BlueBear::Tools {

  CardinalDirection getOpposingDirection( const CardinalDirection& direction ) {
    switch ( direction ) {
      case CardinalDirection::North:
        return CardinalDirection::South;

      case CardinalDirection::South:
        return CardinalDirection::North;

      case CardinalDirection::East:
        return CardinalDirection::West;

      case CardinalDirection::West:
        return CardinalDirection::East;

      case CardinalDirection::Northeast:
        return CardinalDirection::Southwest;

      case CardinalDirection::Southeast:
        return CardinalDirection::Northwest;

      case CardinalDirection::Southwest:
        return CardinalDirection::Northeast;

      case CardinalDirection::Northwest:
        return CardinalDirection::Southeast;
    }
  }

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

}
