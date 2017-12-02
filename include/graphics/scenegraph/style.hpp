#ifndef SG_STYLE
#define SG_STYLE

#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {

        class Shader;
        class Material;

        struct Style {
          std::shared_ptr< Shader > shader;
          std::shared_ptr< Material > material;

          void sendToShader();
        };

    }
  }
}


#endif
