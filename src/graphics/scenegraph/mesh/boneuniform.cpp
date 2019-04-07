#include "graphics/scenegraph/mesh/boneuniform.hpp"
#include "graphics/scenegraph/animation/animator.hpp"
#include "graphics/scenegraph/model.hpp"
#include "tools/opengl.hpp"
#include "log.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        BoneUniform::BoneUniform( const std::vector< std::string >& boneIDs ) :
          boneIDs( boneIDs ) {}

        void BoneUniform::configure( const std::map< std::string, glm::mat4 >& computedBones ) {
          boneUniform.clear();

          // Matrix 0 is always identity
          boneUniform.push_back( glm::mat4() );

          for( const std::string& bone : boneIDs ) {
            boneUniform.push_back( computedBones.find( bone )->second );
          }
        }

        const std::vector< glm::mat4 >& BoneUniform::getBoneList() const {
          return boneUniform;
        }

        void BoneUniform::send() {
          Tools::OpenGL::setUniform( "bones", boneUniform.size(), glm::value_ptr( boneUniform[ 0 ] ) );
        }

      }
    }
  }
}
