#include "graphics/shaderinstancebundle.hpp"
#include "graphics/camera.hpp"

namespace BlueBear {
  namespace Graphics {

    ShaderInstanceBundle::ShaderInstanceBundle( Camera& camera, const std::string& vertexPath, const std::string& fragmentPath ) :
      shader( Shader( vertexPath.c_str(), fragmentPath.c_str() ) ), camera( camera ) {}

    void ShaderInstanceBundle::drawInstances() {
      shader.use();
      camera.sendToShader();

      for( auto& instance : instances ) {
        instance.drawEntity();
      }
    }

  }
}
