#ifndef SG_MESH
#define SG_MESH

#include <GL/glew.h>
#include <vector>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        class Mesh {
          using Index = GLuint;

          GLuint VAO;
          GLuint VBO;
          GLuint EBO;
          unsigned int size;

          // Disable copy
          Mesh( const Mesh& );
          Mesh& operator=( const Mesh& );

        public:
          Mesh();
          virtual ~Mesh();
        };

      }
    }
  }
}

#endif
