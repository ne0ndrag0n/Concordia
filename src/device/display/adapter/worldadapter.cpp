#include "device/display/adapter/worldadapter.hpp"
#include "device/display/display.hpp"

namespace BlueBear {
  namespace Device {
    namespace Display {
      namespace Adapter {

        WorldAdapter::WorldAdapter( Display& display ) : Adapter::Adapter( display ), Component::GuiComponent( display.getRenderWindow() ) {}


        void WorldAdapter::nextFrame() {
          // Render objects
          render();

          // Render GUI
          update();
        }

      }
    }
  }
}
