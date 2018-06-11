#ifndef SG_ANIMATION_ANIMATOR
#define SG_ANIMATION_ANIMATOR

#include "graphics/scenegraph/animation/animation.hpp"
#include "graphics/scenegraph/animation/bone.hpp"
#include <memory>
#include <string>
#include <optional>
#include <map>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Animation {

        // This will sit on the root node of the model to be animated
        // So be careful when pulling apart animated models
        class Animator {
          Bone bindSkeleton;
          Bone currentSkeleton;

          std::optional< Animation > animation;
          std::map< std::string, Animation > animationList;

          double frame = 0.0f;
          bool paused = false;

          double getFPS();

        public:
          EXCEPTION_TYPE( AnimationNotFoundException, "Animation ID not found!" );
          Animator( const Animator& animator );
          Animator(
            const Bone& bindSkeleton,
            const Bone& currentSkeleton,
            const std::map< std::string, Animation >& animationList
          );

          Bone& getBindSkeletonRef();
          Bone& getCurrentSkeletonRef();

          void setCurrentAnimation( const std::string& animationId );
          void setPause( bool status );
          void setFrame( double frame );
          void reset();
          void update();
        };

      }
    }
  }
}

#endif
