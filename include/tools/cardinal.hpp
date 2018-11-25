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

  struct Directional {
    glm::ivec2 remaining;
    glm::ivec2 direction;
    glm::ivec2 start;
    glm::ivec2 end;
    CardinalDirection cardinal;
  };

  extern Directional getDirectional( const glm::ivec2& start, const glm::ivec2& end );
  extern CardinalDirection getOpposingDirection( const CardinalDirection& direction );
  extern CardinalDirection getCardinalDirection( const glm::ivec2& direction );

}

#endif
