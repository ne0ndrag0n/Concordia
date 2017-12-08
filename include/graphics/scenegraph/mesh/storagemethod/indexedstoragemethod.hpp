#ifndef SG_INDEXED_STORAGE_METHOD
#define SG_INDEXED_STORAGE_METHOD

#include <GL/glew.h>
#include <vector>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {
        namespace StorageMethod {

          class IndexedStorageMethod {
            static void allocateIndices( GLuint* EBO );
            static void deallocateIndices( GLuint* EBO );
            static void uploadIndices( GLuint EBO, const std::vector< GLuint >& data );

            static void draw( unsigned int size );
          };

        }
      }
    }
  }
}

#endif
