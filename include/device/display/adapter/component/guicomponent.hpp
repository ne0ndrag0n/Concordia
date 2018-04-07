#ifndef ADAPTER_COMPONENT_GUI
#define ADAPTER_COMPONENT_GUI

#include "device/input/input.hpp"
#include "device/display/adapter/adapter.hpp"
#include "graphics/vector/renderer.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/style/styleapplier.hpp"
#include "graphics/shader.hpp"
#include <queue>
#include <memory>
#include <functional>

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
            Graphics::UserInterface::Style::StyleApplier styleManager;

          public:

            void __testadd();
            void __teststyle();

            std::shared_ptr< Graphics::UserInterface::Element > captureMouseEvent(
              std::shared_ptr< Graphics::UserInterface::Element > element,
              Device::Input::Input::Metadata event
            );

            void mousePressed( Device::Input::Input::Metadata event );
            void mouseReleased( Device::Input::Input::Metadata event );

            Graphics::Vector::Renderer& getVectorRenderer();
            Graphics::UserInterface::Style::StyleApplier& getStyleManager();

            GuiComponent( Device::Display::Display& display );
            void nextFrame() override;
          };

        }
      }
    }
  }
}

#endif
