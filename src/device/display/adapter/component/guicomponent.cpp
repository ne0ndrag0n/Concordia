#include "device/display/adapter/component/guicomponent.hpp"
#include "device/display/display.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <SFGUI/Widget.hpp>
#include <SFGUI/Entry.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace BlueBear {
  namespace Device {
    namespace Display {
      namespace Adapter {
        namespace Component {

          GuiComponent::GuiComponent( Device::Display::Display& display ) :
            Adapter::Adapter( display ),
            vector( display ),
            guiShader( "system/shaders/gui/vertex.glsl", "system/shaders/gui/fragment.glsl" ) {}

          Graphics::Vector::Renderer& GuiComponent::getVectorRenderer() {
            return vector;
          }

          /**
           * Getting the feeling that the only reason we do this is because it's awkward to pass a "renderwindow" to inputmanager
           * when really it shouldn't be...
           */
          std::queue< sf::Event > GuiComponent::getEvents() {
            sf::RenderWindow& renderWindow = display.getRenderWindow();
            std::queue< sf::Event > events;

            sf::Event event;
            while( renderWindow.pollEvent( event ) ) {
              events.push( event );
            }

            return events;
          }

          void GuiComponent::nextFrame() {}

        }
      }
    }
  }
}