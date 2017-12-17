#ifndef SG_BASIC_VERTEX
#define SG_BASIC_VERTEX

#include <glm/glm.hpp>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      namespace Mesh {

        struct BasicVertex {
          glm::vec3 position;
          glm::vec3 normal;

          static void setupShaderAttributes();
        };

      }
    }
  }
}

#endif
