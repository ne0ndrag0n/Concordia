#ifndef ADAPTER_COMPONENT_GUI
#define ADAPTER_COMPONENT_GUI

#include "device/display/adapter/adapter.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <queue>

namespace BlueBear {
  namespace Device {
    namespace Display {
      class Display;

      namespace Adapter {
        namespace Component {

          class GuiComponent : public Adapter {

          public:
            std::queue< sf::Event > getEvents();
            GuiComponent( Device::Display::Display& display );
            void nextFrame() override;
          };

        }
      }
    }
  }
}

#endif
