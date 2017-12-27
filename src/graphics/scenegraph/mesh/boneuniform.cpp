#include "graphics/scenegraph/mesh/boneuniform.hpp"
#include "graphics/scenegraph/animation/animator.hpp"
#include "graphics/scenegraph/model.hpp"
#include "tools/opengl.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        BoneUniform::BoneUniform( const std::vector< std::string >& boneIDs, std::shared_ptr< Animation::Animator > animator ) :
          boneIDs( boneIDs ), animator( animator ) {}

        void BoneUniform::send() {
          std::vector< glm::mat4 > boneUniform;

          // Matrix 0 is always identity
          boneUniform.push_back( glm::mat4() );

          std::shared_ptr< Animation::Animator > realAnimator = animator.lock();
          if( realAnimator ) {
            for( const std::string& bone : boneIDs ) {
              boneUniform.push_back(
                realAnimator->getCurrentSkeletonRef().getMatrixById( bone ) * glm::inverse( realAnimator->getBindSkeletonRef().getMatrixById( bone ) )
              );
            }
          } else {
            for( const std::string& bone : boneIDs ) {
              // Unable to obtain an animator for some reason; we can only roll out a bunch of identity matrices
              boneUniform.push_back( glm::mat4() );
            }
          }

          Tools::OpenGL::lock( [ & ]() {
            glUniformMatrix4fv( Tools::OpenGL::getUniformLocation( "bones" ), boneUniform.size(), GL_FALSE, glm::value_ptr( boneUniform[ 0 ] ) );
          } );
        }

      }
    }
  }
}
