#ifndef CARDINAL_DIRECTION
#define CARDINAL_DIRECTION

#include <glm/glm.hpp>

namespace BlueBear::Tools {

  enum class CardinalDirection {
    North,        // 0, -
    South,        // 0, +
    East,         // +, 0
    West,         // -, 0
    Northeast,    // +, -
    Southeast,    // +, +
    Southwest,    // -, +
    Northwest     // -, -
  };

  extern CardinalDirection getOpposingDirection( const CardinalDirection& direction );
  extern CardinalDirection getCardinalDirection( const glm::ivec2& direction );

}

#endif
