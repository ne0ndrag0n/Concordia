#include "graphics/scenegraph/mesh/boneuniform.hpp"
#include "graphics/scenegraph/animation/animator.hpp"
#include "graphics/scenegraph/model.hpp"
#include "tools/opengl.hpp"
#include "log.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#include "graphics/scenegraph/transform.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        BoneUniform::BoneUniform( const std::vector< std::string >& boneIDs ) :
          boneIDs( boneIDs ) {}

        void BoneUniform::configure( std::shared_ptr< Animation::Animator > animator ) {
          boneUniform.clear();

          // Matrix 0 is always identity
          boneUniform.push_back( glm::mat4() );

          if( animator ) {
            for( const std::string& bone : boneIDs ) {
              boneUniform.push_back(
                animator->getCurrentSkeletonRef().getMatrixById( bone ) * glm::inverse( animator->getBindSkeletonRef().getMatrixById( bone ) )
              );
            }
          } else {
            Log::getInstance().warn( "BoneUniform::configure", "Unable to obtain an animator; sending identity matrices to shader" );

            for( const std::string& bone : boneIDs ) {
              // Unable to obtain an animator for some reason; we can only roll out a bunch of identity matrices
              boneUniform.push_back( glm::mat4() );
            }
          }
        }

        void BoneUniform::send() {
          glUniformMatrix4fv( Tools::OpenGL::getUniformLocation( "bones" ), boneUniform.size(), GL_FALSE, glm::value_ptr( boneUniform[ 0 ] ) );
        }

      }
    }
  }
}
