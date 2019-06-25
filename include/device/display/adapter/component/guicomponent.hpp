#ifndef ADAPTER_COMPONENT_GUI
#define ADAPTER_COMPONENT_GUI

#include "device/display/adapter/adapter.hpp"
#include "graphics/vector/renderer.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/style/styleapplier.hpp"
#include "graphics/shader.hpp"
#include "eventmanager.hpp"
#include <sol.hpp>
#include <vector>
#include <memory>
#include <set>
#include <functional>
#include <unordered_map>

namespace BlueBear {
  namespace Device::Input {
    class Input;
    class Metadata;
  }

  namespace Graphics::UserInterface {
    class DragHelper;
    class Element;
  }

  namespace Graphics::Utilities {
    class ShaderManager;
  }

  namespace Device {
    namespace Display {
      class Display;

      namespace Adapter {
        namespace Component {

          class GuiComponent : public Adapter {
            Graphics::Vector::Renderer vector;
            std::shared_ptr< Graphics::Shader > guiShader;
            std::unique_ptr< Graphics::UserInterface::DragHelper > currentDrag;
            std::set< std::shared_ptr< Graphics::UserInterface::Element > > previousMove;
            std::shared_ptr< Graphics::UserInterface::Element > rootElement;
            std::shared_ptr< Graphics::UserInterface::Element > currentFocus;
            Graphics::UserInterface::Style::StyleApplier styleManager;
            std::unordered_map< std::string, std::function< void( Device::Input::Metadata ) > > blockingGlobalEvents;

            void submitLuaContributions( sol::state& lua );
            void fireFocusEvent( std::shared_ptr< Graphics::UserInterface::Element > selected, Device::Input::Metadata event );
            void fireInOutEvents( std::shared_ptr< Graphics::UserInterface::Element > selected, Device::Input::Metadata event );

            std::shared_ptr< Graphics::UserInterface::Element > captureMouseEvent(
              std::shared_ptr< Graphics::UserInterface::Element > element,
              Device::Input::Metadata event
            );

            void mousePressed( Device::Input::Metadata event );
            void mouseMoved( Device::Input::Metadata event );
            void mouseReleased( Device::Input::Metadata event );
            void keyPressed( Device::Input::Metadata event );
            void keyReleased( Device::Input::Metadata event );

          public:
            BasicEvent< void*, std::shared_ptr< Graphics::UserInterface::Element > > GUI_OBJECT_MOUSE_DOWN;
            BasicEvent< void*, std::shared_ptr< Graphics::UserInterface::Element > > GUI_OBJECT_MOUSE_UP;

            void loadStylesheets( const std::vector< std::string >& paths );
            std::vector< std::shared_ptr< Graphics::UserInterface::Element > > addElementsFromXML( const std::string& xmlPath, bool file = true );
            void addElement( std::shared_ptr< Graphics::UserInterface::Element > element );
            std::vector< std::shared_ptr< Graphics::UserInterface::Element > > query( sol::table queries );

            void removeElement( std::shared_ptr< Graphics::UserInterface::Element > element );

            void setupBlockingGlobalEvent( const std::string& eventId, std::function< void( Device::Input::Metadata ) > callback );
            void unregisterBlockingGlobalEvent( const std::string& eventId );

            void registerEvents( Device::Input::Input& inputManager );

            Graphics::Vector::Renderer& getVectorRenderer();
            Graphics::UserInterface::Style::StyleApplier& getStyleManager();

            void startDrag( std::shared_ptr< Graphics::UserInterface::Element > target, const glm::ivec2& offset );

            GuiComponent( Device::Display::Display& display, Graphics::Utilities::ShaderManager& shaderManager );
            ~GuiComponent();
            void nextFrame() override;
          };

        }
      }
    }
  }
}

#endif
