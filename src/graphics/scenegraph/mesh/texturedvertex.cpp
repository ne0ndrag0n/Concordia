#include "graphics/scenegraph/mesh/texturedvertex.hpp"
#include "graphics/shader.hpp"
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        void TexturedVertex::setupShaderAttributes() {
          glEnableVertexAttribArray( 0 );
          glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( TexturedVertex ), ( GLvoid* ) 0 );

          glEnableVertexAttribArray( 1 );
          glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( TexturedVertex ), ( GLvoid* ) offsetof( TexturedVertex, normal ) );

          glEnableVertexAttribArray( 2 );
          glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof( TexturedVertex ), ( GLvoid* ) offsetof( TexturedVertex, textureCoordinates ) );
        }

        std::pair< std::string, std::string > TexturedVertex::getDefaultShader() {
          return std::pair< std::string, std::string >(
            "system/shaders/textured_vertex_nolight/vertex.glsl",
            "system/shaders/textured_vertex_nolight/fragment.glsl"
          );
        }

      }
    }
  }
}
