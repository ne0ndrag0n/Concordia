#ifndef SG_MESH
#define SG_MESH

#include <functional>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {
      namespace Mesh {

        class Mesh {
        public:
          std::function< std::shared_ptr< Shader >() > getDefaultShader;
          virtual void drawElements() = 0;
        };

      }
    }
  }
}

#endif
