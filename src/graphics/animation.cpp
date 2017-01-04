#include "graphics/animation.hpp"
#include "log.hpp"
#include <string>

namespace BlueBear {
  namespace Graphics {

    Animation::Animation( double rate, double duration, const glm::mat4& inverseBase ) : rate( rate ), duration( duration ), inverseBase( inverseBase ) {}

    void Animation::addKeyframe( double frame, const Transform& transform ) {
      keyframes.emplace( frame, transform );
    }

    glm::mat4 Animation::getTransformForFrame( double frame ) {
      unsigned long iPart = ( unsigned long ) frame;
      double fPart = frame - iPart;

      if( fPart == 0 ) {
        // No interpolation required
      } else {
        // Interpolate between nearest keyframes
        // Begin with keyframes[ iPart ] and interpolate up to keyframes[ iPart + 1 ], using fPart as the alpha
      }
    }
  }
}
