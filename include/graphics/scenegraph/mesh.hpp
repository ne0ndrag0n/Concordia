#ifndef SG_MESH
#define SG_MESH

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext.hpp>
#include <GL/glew.h>
#include <vector>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

      class Mesh {
        // Disallow copy
        Mesh( const Mesh& );
        Mesh& operator=( const Mesh& );

      protected:
        GLuint VAO;
        GLuint VBO;
        GLuint EBO;
        unsigned int size;

      public:
        struct Vertex {
          glm::vec3 position;
          glm::vec3 normal;
        };
        using Index = GLuint;

        Mesh( std::vector< Vertex >& vertices, std::vector< Index >& indices );
        Mesh() = default;
        virtual ~Mesh();

        void draw();
      };

    }
  }
}


#endif
