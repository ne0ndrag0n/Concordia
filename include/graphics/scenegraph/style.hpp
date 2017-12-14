#ifndef SG_STYLE
#define SG_STYLE

#include <memory>

namespace BlueBear {
  namespace Graphics {
    class Shader;

    namespace SceneGraph {

        class Material;

        struct Style {
          std::shared_ptr< Shader > shader;
          std::shared_ptr< Material > material;
        };

    }
  }
}


#endif
