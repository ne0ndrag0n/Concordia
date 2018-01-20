#ifndef ADAPTER_COMPONENT_GUI
#define ADAPTER_COMPONENT_GUI

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Desktop.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

namespace BlueBear {
  namespace Device {
    namespace Display {
      namespace Adapter {
        namespace Component {

          class GuiComponent {
            sf::RenderWindow& renderWindow;
            sfg::Desktop desktop;
            sfg::SFGUI sfgui;
            sf::Clock clock;

            void enumerateSignals();

          public:
            GuiComponent( sf::RenderWindow& renderWindow );
            void update();
          };

        }
      }
    }
  }
}

#endif
