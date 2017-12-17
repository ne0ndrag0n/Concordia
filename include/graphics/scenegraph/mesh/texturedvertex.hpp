#ifndef SG_TEXTURED
#define SG_TEXTURED

#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        struct TexturedVertex {
          glm::vec3 position;
          glm::vec3 normal;
          glm::vec2 textureCoordinates;

          static void setupShaderAttributes();
        };

      }
    }
  }
}


#endif
