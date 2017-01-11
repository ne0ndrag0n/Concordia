#ifndef WORLDOBJECT
#define WORLDOBJECT

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace BlueBear {
  namespace Graphics {
    struct Vertex;

    using Index = GLuint;

    class Mesh {
        // Meshes depend on OpenGL global states - You really shouldn't be copying 'em.
        Mesh( const Mesh& ) = delete;
        Mesh& operator=( const Mesh& ) = delete;

      protected:
        GLuint VAO, VBO, EBO;
        unsigned int size;
        virtual void sendMetadataToShader() = 0;

      public:
        Mesh( unsigned int size );
        virtual ~Mesh();
        void drawElements();
    };

    class StandardMesh : public Mesh {
    public:
      StandardMesh( std::vector< Vertex >& vertices, std::vector< Index >& indices );
    protected:
      virtual void sendMetadataToShader();
    };
  }
}
#endif
