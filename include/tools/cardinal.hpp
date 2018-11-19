#ifndef CARDINAL_DIRECTION
#define CARDINAL_DIRECTION

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

}

#endif
