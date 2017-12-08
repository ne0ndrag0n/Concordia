#ifndef SG_SKELETAL_TEXTURED_VERTEX
#define SG_SKELETAL_TEXTURED_VERTEX

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {
        namespace VertexType {

          class SkeletalTexturedVertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 textureCoordinates;
            glm::ivec4 boneIDs = glm::ivec4( 0, 0, 0, 0 );
            glm::vec4 boneWeights = glm::vec4( 1.0f, 0.0f, 0.0f, 0.0f );

            static void setupShaderAttributes(
              GLuint VBO,
              GLuint EBO,
              const std::vector< SkeletalTexturedVertex >& vertices,
              const std::vector< GLuint >& indices
            );
          };

        }
      }
    }
  }
}


#endif
