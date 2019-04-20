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

          std::map< std::string, glm::mat4 > computedMatrices;

          double frame = 0.0f;
          bool paused = false;

          double getFPS();
          void computeMatrices();

        public:
          EXCEPTION_TYPE( AnimationNotFoundException, "Animation ID not found!" );
          Animator( const Animator& animator );
          Animator(
            const Bone& bindSkeleton,
            const Bone& currentSkeleton,
            const std::map< std::string, Animation >& animationList
          );

          const std::map< std::string, glm::mat4 >& getComputedMatrices();

          Bone& getBindSkeletonRef();
          Bone& getCurrentSkeletonRef();

          bool updating() const;

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
