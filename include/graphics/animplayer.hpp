#ifndef ANIMPLAYER
#define ANIMPLAYER

#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {
    class Animation;

    class AnimPlayer {
      Animation& animation;
      double interval;
      double step = 0.0;

    public:
      glm::mat4 nextMatrix;

      AnimPlayer( Animation& animation );
      void reset();
      bool generateNextFrame();
    };

  }
}


#endif
