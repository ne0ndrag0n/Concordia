#ifndef BB_OBJECT_COMPONENT
#define BB_OBJECT_COMPONENT

#include "graphics/world/renderer.hpp"
#include "graphics/camera.hpp"
#include <glm/glm.hpp>
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
            Graphics::Camera camera;
            Graphics::World::Renderer renderer;

          public:
            ObjectComponent( const glm::uvec2& screenDimensions );
            void update();
          };

        }
      }
    }
  }
}

#endif
