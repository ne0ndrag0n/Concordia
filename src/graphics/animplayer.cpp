#include "graphics/animplayer.hpp"
#include "graphics/animation.hpp"
#include "configmanager.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {

    // Currently configured for a fixed FPS
    AnimPlayer::AnimPlayer( Animation& animation ) :
      animation( animation ), interval( animation.rate / ConfigManager::getInstance().getIntValue( "fps_overview" ) ) {}

    void AnimPlayer::reset() {
      step = 0.0;
    }

    bool AnimPlayer::generateNextFrame() {

      step += interval;
      bool generatable = step <= animation.duration;

      if( generatable ) {
        nextMatrix = animation.getTransformForFrame( step );
      }

      return generatable;
    }
  }
}
