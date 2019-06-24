#ifndef SG_BASIC_VERTEX
#define SG_BASIC_VERTEX

#include <glm/glm.hpp>
#include <memory>
#include <utility>
#include <string>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {
      namespace Mesh {

        struct BasicVertex {
          glm::vec3 position;
          glm::vec3 normal;

          bool operator==( const BasicVertex& rhs ) const;

          static void setupShaderAttributes();
          static std::pair< std::string, std::string > getDefaultShader();
        };

      }
    }
  }
}

#endif
