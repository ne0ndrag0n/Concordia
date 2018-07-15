#ifndef WALLJOINT
#define WALLJOINT

namespace BlueBear::Models {

  struct WallJoint {
    bool north = false;
    bool east = false;
    bool west = false;
    bool south = false;

    bool isSingleAxis() const;
    bool isElbow() const;
    bool isCross() const;
    bool isFull() const;
  };

}

#endif
