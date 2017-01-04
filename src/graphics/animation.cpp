#include "graphics/animation.hpp"

namespace BlueBear {
  namespace Graphics {

    Keyframe::Keyframe( double frame, glm::mat4 transform ) : frame( frame ), transform( transform ) {}

  }
}
