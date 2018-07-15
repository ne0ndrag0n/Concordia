#include "models/walljoint.hpp"

namespace BlueBear::Models {

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

}
