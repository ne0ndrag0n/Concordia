#ifndef SG_TEXTURED_RIGGED
#define SG_TEXTURED_RIGGED

#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        struct TexturedRiggedVertex {
          glm::vec3 position;
          glm::vec3 normal;
          glm::vec2 textureCoordinates;
          glm::ivec4 boneIDs = glm::ivec4( 0, 0, 0, 0 );
          glm::vec4 boneWeights = glm::vec4( 1.0f, 0.0f, 0.0f, 0.0f );

          static void setupShaderAttributes();
        };

      }
    }
  }
}


#endif
