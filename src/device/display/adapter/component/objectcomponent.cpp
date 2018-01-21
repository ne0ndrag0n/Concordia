#include "device/display/adapter/component/objectcomponent.hpp"
#include "graphics/scenegraph/model.hpp"

namespace BlueBear {
  namespace Device {
    namespace Display {
      namespace Adapter {
        namespace Component {

          ObjectComponent::ObjectComponent( const glm::uvec2& screenDimensions ) :
            camera( Graphics::Camera( screenDimensions.x, screenDimensions.y ) ) {

            }

          void ObjectComponent::update() {
            camera.position();

            renderer.render();
          }

        }
      }
    }
  }
}
