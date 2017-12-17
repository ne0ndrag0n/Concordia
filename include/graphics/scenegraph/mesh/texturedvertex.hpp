#ifndef SG_TEXTURED
#define SG_TEXTURED

#include <glm/glm.hpp>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {
      namespace Mesh {

        struct TexturedVertex {
          glm::vec3 position;
          glm::vec3 normal;
          glm::vec2 textureCoordinates;

          static void setupShaderAttributes();
          static std::shared_ptr< Shader > getDefaultShader();
        };

      }
    }
  }
}


#endif
