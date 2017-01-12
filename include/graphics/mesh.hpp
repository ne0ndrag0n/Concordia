#ifndef WORLDOBJECT
#define WORLDOBJECT

// Do not include model!
// If you need to include Model in this file, BoneList needs its own file.
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    class Model;

    struct Vertex {
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 textureCoordinates;
    };
    using Index = GLuint;
    using BoneList = std::vector< std::shared_ptr< Model > >;

    class Mesh {
      private:
        GLuint VAO, VBO, EBO;
        unsigned int size;
        // Meshes depend on OpenGL global states - You really shouldn't be copying 'em.
        Mesh( const Mesh& );
        Mesh& operator=( const Mesh& );

        virtual void sendMetadataToShader();

      public:
        Mesh( std::vector< Vertex >& vertices, std::vector< Index >& indices );
        virtual ~Mesh();
        void setupMesh( std::vector< Vertex >& vertices, std::vector< Index >& indices );
        void drawElements();
    };
  }
}
#endif
