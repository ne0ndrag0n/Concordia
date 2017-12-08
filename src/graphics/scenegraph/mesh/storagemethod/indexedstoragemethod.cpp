#include "graphics/scenegraph/mesh/storagemethod/indexedstoragemethod.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {
        namespace StorageMethod {

          void IndexedStorageMethod::allocate( GLuint* VAO, GLuint* VBO, GLuint* EBO ) {
            glGenVertexArrays( 1, VAO );
            glGenBuffers( 1, VBO );
            glGenBuffers( 1, EBO );
          }

          void IndexedStorageMethod::deallocate( GLuint* VAO, GLuint* VBO, GLuint* EBO ) {
            glDeleteVertexArrays( 1, VAO );
            glDeleteBuffers( 1, VBO );
            glDeleteBuffers( 1, EBO );
          }

          void IndexedStorageMethod::draw( unsigned int size ) {
            glDrawElements( GL_TRIANGLES, size, GL_UNSIGNED_INT, 0 );
          }

        }
      }
    }
  }
}
