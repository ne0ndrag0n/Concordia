#ifndef ANIMPLAYER
#define ANIMPLAYER

#include "graphics/transform.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    class KeyframeBundle;
    class Transform;

    class AnimPlayer {
      KeyframeBundle& animation;
      double interval;
      double step = 0.0;

    public:
      AnimPlayer( KeyframeBundle& animation );
      void reset();
      std::shared_ptr< Transform > generateNextFrame();
    };

  }
}


#endif
