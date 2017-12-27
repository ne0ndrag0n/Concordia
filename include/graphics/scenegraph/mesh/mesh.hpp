#ifndef SG_MESH
#define SG_MESH

#include "graphics/scenegraph/uniform.hpp"
#include <functional>
#include <memory>
#include <string>
#include <map>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {
      namespace Mesh {

        class Mesh {
        public:
          std::map< std::string, std::shared_ptr< Uniform > > meshUniforms;
          std::function< std::shared_ptr< Shader >() > getDefaultShader;
          virtual void drawElements() = 0;
        };

      }
    }
  }
}

#endif
