#ifndef ANIMPLAYER
#define ANIMPLAYER

#include "graphics/transform.hpp"
#include "graphics/keyframebundle.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <string>

namespace BlueBear {
  namespace Graphics {
    class Transform;
    class Armature;

    class AnimPlayer {
      Animation& animation;
      double interval;
      double step = 0.0;
      bool paused = false;

    public:
      AnimPlayer( Animation& animation );
      void reset();
      std::shared_ptr< Armature > generateNextFrame( std::shared_ptr< Armature > bindPose );
      std::shared_ptr< Armature > generateFrame( std::shared_ptr< Armature > bindPose, double step );

      bool getPaused();
      void pause();
    };

  }
}


#endif
