#ifndef BB_OBJECT_COMPONENT
#define BB_OBJECT_COMPONENT

#include "graphics/camera.hpp"
#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace SceneGraph {
      class Model;
    }
  }

  namespace Device {
    namespace Display {
      namespace Adapter {
        namespace Component {

          class ObjectComponent {
            std::shared_ptr< Graphics::SceneGraph::Model > root;
            Camera camera;

          public:
            ObjectComponent();
            void update();
          };

        }
      }
    }
  }
}

#endif
