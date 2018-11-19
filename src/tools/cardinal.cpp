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

}
