#include "graphics/shaderinstancebundle.hpp"
#include "graphics/shader.hpp"
#include "graphics/instance/instance.hpp"
#include "graphics/camera.hpp"

namespace BlueBear {
  namespace Graphics {

    ShaderInstanceBundle::ShaderInstanceBundle( const std::string& vertexPath, const std::string& fragmentPath ) :
      shader( std::make_shared< Shader >( vertexPath.c_str(), fragmentPath.c_str() ) ) {}

    ShaderInstanceBundle::ShaderInstanceBundle( std::shared_ptr< Shader > shader ) :
      shader( shader ) {}

    void ShaderInstanceBundle::drawInstances( Camera& camera ) {
      shader->use();
      camera.sendToShader();

      for( std::shared_ptr< Instance > instance : instances ) {
        instance->drawEntity();
      }
    }

  }
}
