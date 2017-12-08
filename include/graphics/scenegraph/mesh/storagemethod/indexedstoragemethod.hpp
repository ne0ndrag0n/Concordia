#ifndef SG_INDEXED_STORAGE_METHOD
#define SG_INDEXED_STORAGE_METHOD

#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {
        namespace StorageMethod {

          class IndexedStorageMethod {
            static void allocate( GLuint* VAO, GLuint* VBO, GLuint* EBO );
            static void deallocate( GLuint* VAO, GLuint* VBO, GLuint* EBO );
            static void draw( unsigned int size );
          };

        }
      }
    }
  }
}

#endif
