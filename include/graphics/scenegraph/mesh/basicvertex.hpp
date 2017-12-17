#ifndef SG_BASIC_VERTEX
#define SG_BASIC_VERTEX

#include <glm/glm.hpp>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {
      namespace Mesh {

        struct BasicVertex {
          glm::vec3 position;
          glm::vec3 normal;

          static void setupShaderAttributes();
          static std::shared_ptr< Shader > getDefaultShader();
        };

      }
    }
  }
}

#endif
