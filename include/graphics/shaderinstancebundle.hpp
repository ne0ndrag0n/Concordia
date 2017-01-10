#ifndef SHADERINSTANCEBUNDLE
#define SHADERINSTANCEBUNDLE

#include "graphics/shader.hpp"
#include "graphics/instance/instance.hpp"
#include <vector>

namespace BlueBear {
  namespace Graphics {
    class Camera;

    class ShaderInstanceBundle {
    public:
      Shader shader;
      std::vector< Instance > instances;

      ShaderInstanceBundle( const std::string& vertexPath, const std::string& fragmentPath );
      void drawInstances( Camera& camera );
    };

  }
}

#endif
