#ifndef ANIMATION
#define ANIMATION

#include "graphics/transform.hpp"
#include <glm/glm.hpp>
#include <map>

namespace BlueBear {
  namespace Graphics {

    class KeyframeBundle {
    public:
      std::map< double, Transform > keyframes;

      double rate;
      double duration;
      /**
       * Quicker, caches transforms generated but uses more ram
       */
      bool cacheInterpolations = false;

      KeyframeBundle() = default;
      KeyframeBundle( double rate, double duration );

      void addKeyframe( double frame, const Transform& transform );
      glm::mat4 getTransformForFrame( double frame );
    };

  }
}

#endif
