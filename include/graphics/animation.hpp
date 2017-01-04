#ifndef ANIMATION
#define ANIMATION

#include <glm/glm.hpp>
#include <vector>

namespace BlueBear {
  namespace Graphics {

    class Keyframe {
    public:
      double frame;
      glm::mat4 transform;

      Keyframe() = default;
      Keyframe( double frame, glm::mat4 transform );
    };

    class Animation {
    public:
      std::vector< Keyframe > keyframes;
      double rate;
      double duration;

      Animation() = default;
    };

  }
}

#endif
