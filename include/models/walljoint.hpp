#ifndef WALLJOINT
#define WALLJOINT

namespace BlueBear::Models {

  struct WallJoint {
    bool north = false;
    bool east = false;
    bool west = false;
    bool south = false;

    bool isSingleAxis();
    bool isElbow();
    bool isCross();
    bool isFull();
  };

}

#endif
