#ifndef ADAPTER_COMPONENT_GUI
#define ADAPTER_COMPONENT_GUI

#include "device/display/adapter/adapter.hpp"
#include "graphics/vector/renderer.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/shader.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <queue>
#include <memory>

namespace BlueBear {
  namespace Graphics {
    namespace UserInterface {
      class Element;
    }
  }

  namespace Device {
    namespace Display {
      class Display;

      namespace Adapter {
        namespace Component {

          class GuiComponent : public Adapter {
            Graphics::Vector::Renderer vector;
            Graphics::Shader guiShader;
            std::shared_ptr< Graphics::UserInterface::Element > rootElement;

          public:

            void __testadd();

            Graphics::Vector::Renderer& getVectorRenderer();

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
