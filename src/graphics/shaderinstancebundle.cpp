#include "graphics/shaderinstancebundle.hpp"
#include "graphics/camera.hpp"

namespace BlueBear {
  namespace Graphics {

    ShaderInstanceBundle::ShaderInstanceBundle( const std::string& vertexPath, const std::string& fragmentPath ) :
      shader( Shader( vertexPath.c_str(), fragmentPath.c_str() ) ) {}

    void ShaderInstanceBundle::drawInstances( Camera& camera ) {
      shader.use();
      camera.sendToShader( shader );

      for( auto& instance : instances ) {
        instance.drawEntity();
      }
    }

  }
}
