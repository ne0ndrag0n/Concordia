#ifndef ANIMATION
#define ANIMATION

#include "graphics/transform.hpp"
#include <glm/glm.hpp>
#include <map>

namespace BlueBear {
  namespace Graphics {

    class KeyframeBundle {
      std::map< double, Transform > keyframes;
      glm::mat4 inverseBase;

    public:
      double rate;
      double duration;
      /**
       * Quicker, caches transforms generated but uses more ram
       */
      bool cacheInterpolations = false;

      KeyframeBundle() = default;
      KeyframeBundle( double rate, double duration, const glm::mat4& inverseBase );

      void addKeyframe( double frame, const Transform& transform );
      glm::mat4 getTransformForFrame( double frame );
    };

  }
}

#endif
