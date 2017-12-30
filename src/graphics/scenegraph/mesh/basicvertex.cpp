#include "graphics/scenegraph/mesh/basicvertex.hpp"
#include "graphics/shader.hpp"
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        void BasicVertex::setupShaderAttributes() {
          glEnableVertexAttribArray( 0 );
          glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof( BasicVertex ), ( GLvoid* ) 0 );

          glEnableVertexAttribArray( 1 );
          glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof( BasicVertex ), ( GLvoid* ) offsetof( BasicVertex, normal ) );
        }

        std::pair< std::string, std::string > BasicVertex::getDefaultShader() {
          return std::pair< std::string, std::string >(
            "system/shaders/basic_vertex_nolight/vertex.glsl",
            "system/shaders/basic_vertex_nolight/fragment.glsl"
          );
        }

      }
    }
  }
}
