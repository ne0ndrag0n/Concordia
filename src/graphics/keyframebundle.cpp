#include "graphics/keyframebundle.hpp"
#include "log.hpp"
#include <string>
#include <glm/ext.hpp>

namespace BlueBear {
  namespace Graphics {

    KeyframeBundle::KeyframeBundle( double rate, double duration ) : rate( rate ), duration( duration ) {}

    void KeyframeBundle::addKeyframe( double frame, const Transform& transform ) {
      keyframes.emplace( frame, transform );
    }

    std::shared_ptr< Transform > KeyframeBundle::getTransformForFrame( double frame ) {
      auto pair = keyframes.find( frame );

      if( pair != keyframes.end() ) {
        // Use keyframe directly
        return std::make_shared< Transform >( pair->second );
      } else {
        // Need to interpolate between two frames
        auto lastIterator = keyframes.upper_bound( frame );
        auto firstIterator = std::prev( lastIterator, 1 );

        // Interpolate the two animations using the alpha
        return std::make_shared< Transform >( Transform::interpolate( firstIterator->second, lastIterator->second, ( ( frame - firstIterator->first ) / ( lastIterator->first - firstIterator->first ) ) ) );
      }
    }
  }
}
