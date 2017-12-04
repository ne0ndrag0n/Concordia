#include "graphics/scenegraph/mesh/mesh.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        Mesh::Mesh() {
          glGenVertexArrays( 1, &VAO );
          glGenBuffers( 1, &VBO );
          glGenBuffers( 1, &EBO );
        }

        Mesh::~Mesh() {
          glDeleteVertexArrays( 1, &VAO );
          glDeleteBuffers( 1, &VBO );
          glDeleteBuffers( 1, &EBO );
        }

        void Mesh::drawElements() {
          for( auto& pair : meshUniforms ) {
            pair.second->send();
          }

          glBindVertexArray( VAO );
            glDrawElements( GL_TRIANGLES, size, GL_UNSIGNED_INT, 0 );
          glBindVertexArray( 0 );
        }

      }
    }
  }
}
