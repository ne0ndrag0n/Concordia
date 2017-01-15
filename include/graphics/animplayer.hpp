#ifndef ANIMPLAYER
#define ANIMPLAYER

#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {
    class KeyframeBundle;

    class AnimPlayer {
      KeyframeBundle& animation;
      double interval;
      double step = 0.0;

    public:
      glm::mat4 nextMatrix;

      AnimPlayer( KeyframeBundle& animation );
      void reset();
      bool generateNextFrame();
    };

  }
}


#endif
