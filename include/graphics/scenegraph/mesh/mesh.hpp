#ifndef SG_MESH
#define SG_MESH

#include "graphics/scenegraph/mesh/meshuniform.hpp"
#include <GL/glew.h>
#include <vector>
#include <memory>
#include <map>

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
          std::map< std::string, std::unique_ptr< MeshUniform > > meshUniforms;

          Mesh();
          virtual ~Mesh();

          virtual void drawElements();
        };

      }
    }
  }
}

#endif
