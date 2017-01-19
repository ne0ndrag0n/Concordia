#ifndef ANIMATION
#define ANIMATION

#include "graphics/transform.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <map>

namespace BlueBear {
  namespace Graphics {

    class KeyframeBundle {
    public:
      std::map< double, Transform > keyframes;

      double rate;
      double duration;

      KeyframeBundle() = default;
      KeyframeBundle( double rate, double duration );

      void addKeyframe( double frame, const Transform& transform );
      std::shared_ptr< Transform > getTransformForFrame( double frame );
    };

  }
}

#endif
