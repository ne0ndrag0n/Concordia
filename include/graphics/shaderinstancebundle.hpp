#ifndef SHADERINSTANCEBUNDLE
#define SHADERINSTANCEBUNDLE

#include <memory>
#include <vector>

namespace BlueBear {
  namespace Graphics {
    class Camera;
    class Shader;
    class Instance;

    class ShaderInstanceBundle {
    public:
      std::shared_ptr< Shader > shader;
      std::vector< std::shared_ptr< Instance > > instances;

      ShaderInstanceBundle( const std::string& vertexPath, const std::string& fragmentPath );
      ShaderInstanceBundle( std::shared_ptr< Shader > shader );
      void drawInstances( Camera& camera );
    };

  }
}

#endif
