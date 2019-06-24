#ifndef SG_TEXTURED
#define SG_TEXTURED

#include <glm/glm.hpp>
#include <memory>
#include <utility>
#include <string>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {
      namespace Mesh {

        struct TexturedVertex {
          glm::vec3 position;
          glm::vec3 normal;
          glm::vec2 textureCoordinates;

          bool operator==( const TexturedVertex& rhs ) const;

          static void setupShaderAttributes();
          static std::pair< std::string, std::string > getDefaultShader();
        };

      }
    }
  }
}


#endif
