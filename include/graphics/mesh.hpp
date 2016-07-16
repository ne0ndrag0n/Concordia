#ifndef WORLDOBJECT
#define WORLDOBJECT

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {

    struct Vertex {
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 textureCoordinates;
    };
    using Index = GLuint;

    class Mesh {
      private:
        GLuint VAO, VBO, EBO;
        // Meshes depend on OpenGL global states - You really shouldn't be copying 'em.
        Mesh( const Mesh& );
        Mesh& operator=( const Mesh& );
        std::vector< Vertex > vertices;
        std::vector< Index > indices;

      public:
        Mesh( std::vector< Vertex > vertices, std::vector< Index > indices );
        ~Mesh();
        void setupMesh();
        void drawElements();
    };
  }
}
#endif
