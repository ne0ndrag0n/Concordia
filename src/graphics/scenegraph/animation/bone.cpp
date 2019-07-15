#include "graphics/scenegraph/animation/bone.hpp"
#include "graphics/scenegraph/transform.hpp"
#include "tools/utility.hpp"
#include "log.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Animation {

        Bone::Bone( const std::string& id, const glm::mat4& matrix, std::shared_ptr< AnimationMap > animationMap ) :
          id( id ), matrix( matrix ), animations( animationMap ) {}

        Bone& Bone::operator=( const Bone& other ) {
          copyReparent( other );
          return *this;
        }

        Bone::Bone( const Bone& other ) {
          copyReparent( other );
        }

        void Bone::copyReparent( const Bone& other ) {
          id = other.id;
          matrix = other.matrix;
          children = other.children;
          for( Bone& child : children ) {
            child.parent = this;
          }
          animations = other.animations;
        }

        void Bone::setToAnimation( const std::string& animationId, double animationTick ) {
          if( !animations ) {
            throw AnimationNotFoundException();
          }

          auto ksIterator = animations->find( animationId );
          if( ksIterator == animations->end() ) {
            throw AnimationNotFoundException();
          }

          const std::map< double, glm::mat4 >& keyframes = ksIterator->second;
          auto keyframePair = keyframes.find( animationTick );
          if( keyframePair != keyframes.end() ) {
            // Keyframe directly exists: replace the matrix directly
            matrix = keyframePair->second;
          } else {
            // Keyframe must be linearly interpolated between the one before and the next one
            auto lastIterator = keyframes.upper_bound( animationTick );
            auto firstIterator = std::prev( lastIterator, 1 );

            Transform result = Transform::interpolate(
              Transform( firstIterator->second ),
              Transform( lastIterator->second ),
              (
                ( animationTick - firstIterator->first ) /
                ( lastIterator->first - firstIterator->first )
              )
            );

            matrix = result.getMatrix();
          }

          for( Bone& child : children ) {
            child.setToAnimation( animationId, animationTick );
          }
        }

        std::vector< std::string > Bone::getAllIds() const {
          std::vector< std::string > result;

          result.push_back( id );
          for( const Bone& bone : children ) {
            result = Tools::Utility::concatArrays( result, bone.getAllIds() );
          }

          return result;
        }

        void Bone::printToLog() {
          Log::getInstance().debug( "Bone::printToLog", id );
          Transform( matrix ).printToLog();

          for( auto& child : children ) {
            child.printToLog();
          }
        }

        Bone Bone::getAnimationCopy( const std::string& animationId, double animationTick ) const {
          Bone result = *this;

          result.setToAnimation( animationId, animationTick );

          return result;
        }

        void Bone::addChild( const Bone& bone ) {
          Bone& child = *children.insert( children.end(), bone );
          child.parent = this;
        }

        const Bone* Bone::getChildById( const std::string& id ) const {
          if( this->id == id ) {
            return this;
          }

          for( const Bone& child : children ) {
            if( const Bone* result = child.getChildById( id ) ) {
              return result;
            }
          }

          return nullptr;
        }

        glm::mat4 Bone::getLocalMatrix() const {
          return matrix;
        }

        glm::mat4 Bone::getMatrixById( const std::string& id ) const {
          const Bone* result = getChildById( id );
          if( result ) {
            glm::mat4 matrix = result->matrix;

            while( ( result = result->parent ) ) {
              matrix = result->matrix * matrix;
            }

            return matrix;
          } else {
            throw BoneNotFoundException();
          }
        }

      }
    }
  }
}
