#include "tools/cardinal.hpp"
#include "tools/utility.hpp"

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

  Directional getDirectional( const glm::ivec2& start, const glm::ivec2& end ) {
    glm::ivec2 direction = Tools::Utility::normalize( end - start );
    CardinalDirection cardinal = Tools::getCardinalDirection( direction );

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

}
