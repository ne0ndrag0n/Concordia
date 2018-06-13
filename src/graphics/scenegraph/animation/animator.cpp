#include "graphics/scenegraph/animation/animator.hpp"
#include "configmanager.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Animation {

        Animator::Animator( const Bone& bindSkeleton, const Bone& currentSkeleton, const std::map< std::string, Animation >& animationList ) :
          bindSkeleton( bindSkeleton ), currentSkeleton( currentSkeleton ), animationList( animationList ) {}

        Animator::Animator( const Animator& animator ) {
          currentSkeleton = bindSkeleton = animator.bindSkeleton;
          animationList = animator.animationList;
        }

        double Animator::getFPS() {
          return animation->fps / ConfigManager::getInstance().getIntValue( "fps_overview" );
        }

        void Animator::computeMatrices() {
          std::vector< std::string > boneIDs = bindSkeleton.getAllIds();

          for( const std::string& id : boneIDs ) {
            computedMatrices[ id ] = currentSkeleton.getMatrixById( id ) * glm::inverse( bindSkeleton.getMatrixById( id ) );
          }
        }

        const std::map< std::string, glm::mat4 >& Animator::getComputedMatrices() {
          return computedMatrices;
        }

        Bone& Animator::getBindSkeletonRef() {
          return bindSkeleton;
        }

        Bone& Animator::getCurrentSkeletonRef() {
          return currentSkeleton;
        }

        void Animator::setCurrentAnimation( const std::string& animationId ) {
          auto it = animationList.find( animationId );
          if( it != animationList.end() ) {
            animation = it->second;
            frame = -getFPS();
            paused = false;
          } else {
            throw AnimationNotFoundException();
          }
        }

        void Animator::setPause( bool status ) {
          paused = status;
        }

        void Animator::setFrame( double frame ) {
          if( animation ) {
            currentSkeleton = bindSkeleton.getAnimationCopy( animation->id, this->frame = frame );
          } else {
            Log::getInstance().warn( "Animator::setFrame", "Can't set frame; no animation currently set" );
          }
        }

        void Animator::reset() {
          animation.reset();
          frame = 0.0f;
          paused = false;
          currentSkeleton = bindSkeleton;
        }

        void Animator::update() {
          if( animation && !paused ) {
            double currentFps = getFPS();
            if( frame + currentFps <= animation->duration ) {
              frame += currentFps;

              currentSkeleton = bindSkeleton.getAnimationCopy( animation->id, frame );
            } else {
              // That was the last frame
              reset();
            }
          }

          computeMatrices();
        }

      }
    }
  }
}
