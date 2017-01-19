#include "graphics/animplayer.hpp"
#include "graphics/keyframebundle.hpp"
#include "tools/utility.hpp"
#include "configmanager.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {

    // Currently configured for a fixed FPS
    AnimPlayer::AnimPlayer( KeyframeBundle& animation ) :
      animation( animation ), interval( animation.rate / ConfigManager::getInstance().getIntValue( "fps_overview" ) ) {}

    void AnimPlayer::reset() {
      step = 0.0;
    }

    /**
     * AnimPlayer should "suicide" once this is completed (from instance)
     */
    std::shared_ptr< Transform > AnimPlayer::generateNextFrame() {
      step += interval;

      if( step <= animation.duration ) {
        return animation.getTransformForFrame( step );
      } else {
        return std::shared_ptr< Transform >( nullptr );
      }
    }
  }
}
