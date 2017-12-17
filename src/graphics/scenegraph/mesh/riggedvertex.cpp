#include "graphics/scenegraph/mesh/riggedvertex.hpp"
#include "graphics/shader.hpp"
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

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

        std::shared_ptr< Shader > RiggedVertex::getDefaultShader() {
          return std::shared_ptr< Shader >();
        }

      }
    }
  }
}
