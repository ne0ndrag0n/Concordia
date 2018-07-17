#include "graphics/scenegraph/mesh/riggedvertex.hpp"
#include "graphics/shader.hpp"
#include "tools/utility.hpp"
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        bool RiggedVertex::operator==( const RiggedVertex& rhs ) const {
          return Tools::Utility::equalEpsilon( position, rhs.position ) &&
            Tools::Utility::equalEpsilon( normal, rhs.normal ) &&
            boneIDs == rhs.boneIDs &&
            Tools::Utility::equalEpsilon( boneWeights, rhs.boneWeights );
        }

        void RiggedVertex::setupShaderAttributes() {
          glEnableVertexAttribArray( 0 );
          glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( RiggedVertex ), ( GLvoid* ) 0 );

          glEnableVertexAttribArray( 1 );
          glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( RiggedVertex ), ( GLvoid* ) offsetof( RiggedVertex, normal ) );

          glEnableVertexAttribArray( 2 );
          glVertexAttribIPointer( 2, 4, GL_INT, sizeof( RiggedVertex ), ( GLvoid* ) offsetof( RiggedVertex, boneIDs ) );

          glEnableVertexAttribArray( 3 );
          glVertexAttribPointer( 3, 4, GL_FLOAT, GL_FALSE, sizeof( RiggedVertex ), ( GLvoid* ) offsetof( RiggedVertex, boneWeights ) );
        }

        std::pair< std::string, std::string > RiggedVertex::getDefaultShader() {
          return std::pair< std::string, std::string >(
            "system/shaders/rigged_vertex/vertex.glsl",
            "system/shaders/rigged_vertex/fragment.glsl"
          );
        }

      }
    }
  }
}
