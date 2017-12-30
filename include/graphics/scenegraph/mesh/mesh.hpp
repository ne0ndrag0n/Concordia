#ifndef SG_MESH
#define SG_MESH

#include "graphics/scenegraph/uniform.hpp"
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <map>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {
      namespace Mesh {

        class Mesh {
        public:
          std::map< std::string, std::shared_ptr< Uniform > > meshUniforms;
          std::function< std::pair< std::string, std::string >() > getDefaultShader;

          virtual ~Mesh() = default;

          virtual void sendDeferred() = 0;
          virtual void drawElements() = 0;
        };

      }
    }
  }
}

#endif
