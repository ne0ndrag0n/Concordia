#ifndef ADAPTER_COMPONENT_GUI
#define ADAPTER_COMPONENT_GUI

#include "device/display/adapter/adapter.hpp"
#include "graphics/vector/renderer.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/style/styleapplier.hpp"
#include "graphics/shader.hpp"
#include <queue>
#include <memory>
#include <functional>

namespace BlueBear {
  namespace Device::Input {
    class Metadata;
  }

  namespace Graphics::UserInterface {
    class DragHelper;
    class Element;
  }

  namespace Device {
    namespace Display {
      class Display;

      namespace Adapter {
        namespace Component {

          class GuiComponent : public Adapter {
            Graphics::Vector::Renderer vector;
            Graphics::Shader guiShader;
            std::unique_ptr< Graphics::UserInterface::DragHelper > currentDrag;
            std::shared_ptr< Graphics::UserInterface::Element > rootElement;
            Graphics::UserInterface::Style::StyleApplier styleManager;

          public:

            void __testadd();
            void __teststyle();

            std::shared_ptr< Graphics::UserInterface::Element > captureMouseEvent(
              std::shared_ptr< Graphics::UserInterface::Element > element,
              Device::Input::Metadata event
            );

            void mousePressed( Device::Input::Metadata event );
            void mouseMoved( Device::Input::Metadata event );
            void mouseReleased( Device::Input::Metadata event );

            Graphics::Vector::Renderer& getVectorRenderer();
            Graphics::UserInterface::Style::StyleApplier& getStyleManager();

            void startDrag( std::shared_ptr< Graphics::UserInterface::Element > target, const glm::ivec2& offset );

            GuiComponent( Device::Display::Display& display );
            ~GuiComponent();
            void nextFrame() override;
          };

        }
      }
    }
  }
}

#endif
