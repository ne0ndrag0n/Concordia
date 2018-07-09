#include "models/walljoint.hpp"

namespace BlueBear::Models {

  bool WallJoint::isSingleAxis() {
    return ( ( north || south ) && !( east || west ) ) ||
           ( ( east || west ) && !( north || south ) );
  }

  bool WallJoint::isElbow() {
    return ( north || south ) && ( east || west );
  }

  bool WallJoint::isCross() {
    return
      ( ( north && south ) && ( east && !west ) ) ||
      ( ( north && south ) && ( !east && west ) ) ||
      ( ( east && west ) && ( north && !south ) ) ||
      ( ( east && west ) && ( !north && south ) );
  }

  bool WallJoint::isFull() {
    return north && south && east && west;
  }

}
