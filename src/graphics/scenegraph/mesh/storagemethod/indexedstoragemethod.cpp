#include "graphics/scenegraph/mesh/storagemethod/indexedstoragemethod.hpp"

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {
        namespace StorageMethod {

          void IndexedStorageMethod::allocateIndices( GLuint* EBO ) {
            glGenBuffers( 1, EBO );
          }

          void IndexedStorageMethod::deallocateIndices( GLuint* EBO ) {
            glDeleteBuffers( 1, EBO );
          }

          void IndexedStorageMethod::draw( unsigned int size ) {
            glDrawElements( GL_TRIANGLES, size, GL_UNSIGNED_INT, 0 );
          }

          void IndexedStorageMethod::uploadIndices( GLuint EBO, const std::vector< GLuint >& data ) {
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
            glBufferData( GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof( GLuint ), &data[ 0 ], GL_STATIC_DRAW );
          }

        }
      }
    }
  }
}
