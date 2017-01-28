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
    class Armature;
    class Model;

    struct Vertex {
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 textureCoordinates;
      glm::ivec4 boneIDs = glm::ivec4( 0, 0, 0, 0 );
      glm::vec4 boneWeights = glm::vec4( 1.0f, 0.0f, 0.0f, 0.0f );
    };
    using Index = GLuint;

    class Mesh {
      private:
        GLuint VAO, VBO, EBO;
        unsigned int size;

        std::vector< std::string > boneIndices;
        std::shared_ptr< Armature > inverseBind;

        // Meshes depend on OpenGL global states - You really shouldn't be copying 'em.
        Mesh( const Mesh& );
        Mesh& operator=( const Mesh& );

      public:
        Mesh(
          std::vector< Vertex >& vertices,
          std::vector< Index >& indices,
          std::vector< std::string > boneIndices,
          std::shared_ptr< Armature > inverseBind
        );
        virtual ~Mesh();
        void setupMesh( std::vector< Vertex >& vertices, std::vector< Index >& indices );
        void drawElements( std::shared_ptr< Armature > currentPose = nullptr );
    };
  }
}
#endif
