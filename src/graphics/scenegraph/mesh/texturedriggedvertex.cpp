#include "graphics/scenegraph/mesh/texturedriggedvertex.hpp"
#include "graphics/shader.hpp"
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        void TexturedRiggedVertex::setupShaderAttributes() {
          glEnableVertexAttribArray( 0 );
          glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( TexturedRiggedVertex ), ( GLvoid* ) 0 );

          glEnableVertexAttribArray( 1 );
          glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( TexturedRiggedVertex ), ( GLvoid* ) offsetof( TexturedRiggedVertex, normal ) );

          glEnableVertexAttribArray( 2 );
          glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( TexturedRiggedVertex ), ( GLvoid* ) offsetof( TexturedRiggedVertex, textureCoordinates ) );

          glEnableVertexAttribArray( 3 );
          glVertexAttribIPointer( 3, 4, GL_INT, sizeof( TexturedRiggedVertex ), ( GLvoid* ) offsetof( TexturedRiggedVertex, boneIDs ) );

          glEnableVertexAttribArray( 4 );
          glVertexAttribPointer( 4, 4, GL_FLOAT, GL_FALSE, sizeof( TexturedRiggedVertex ), ( GLvoid* ) offsetof( TexturedRiggedVertex, boneWeights ) );
        }

        std::pair< std::string, std::string > TexturedRiggedVertex::getDefaultShader() {
          return std::pair< std::string, std::string >(
            "system/shaders/textured_rigged_vertex/vertex.glsl",
            "system/shaders/textured_rigged_vertex/fragment.glsl"
          );
        }

      }
    }
  }
}
