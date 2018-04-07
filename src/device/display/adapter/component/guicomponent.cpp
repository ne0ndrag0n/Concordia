#include "device/display/adapter/component/guicomponent.hpp"
#include "device/display/display.hpp"
#include "graphics/userinterface/element.hpp"
#include "graphics/userinterface/widgets/layout.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/drawable.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "graphics/userinterface/widgets/window.hpp"
#include <glm/gtx/string_cast.hpp>
#include "tools/utility.hpp"

namespace BlueBear {
  namespace Device {
    namespace Display {
      namespace Adapter {
        namespace Component {

          GuiComponent::GuiComponent( Device::Display::Display& display ) :
            Adapter::Adapter( display ),
            vector( display ),
            guiShader( "system/shaders/gui/vertex.glsl", "system/shaders/gui/fragment.glsl" ),
            rootElement( Graphics::UserInterface::Widgets::Layout::create( "", {} ) ),
            styleManager( rootElement ) {
              Graphics::UserInterface::Element::manager = this;

              rootElement->getPropertyList().set< int >( "top", 0, false );
              rootElement->getPropertyList().set< int >( "left", 0, false );
              rootElement->getPropertyList().set< int >( "width", ConfigManager::getInstance().getIntValue( "viewport_x" ), false );
              rootElement->getPropertyList().set< int >( "height", ConfigManager::getInstance().getIntValue( "viewport_y" ), false );
              rootElement->getPropertyList().set< Graphics::UserInterface::Gravity >( "gravity", Graphics::UserInterface::Gravity::TOP, false );
              rootElement->setAllocation( {
                0,
                0,
                ConfigManager::getInstance().getIntValue( "viewport_x" ),
                ConfigManager::getInstance().getIntValue( "viewport_y" )
              } );

              styleManager.applyStyles( {
                "system/ui/system.style"
              } );
            }

          // TODO: remove TEST code
          void GuiComponent::__testadd() {
            auto window = Graphics::UserInterface::Widgets::Window::create( "test", {}, "Window Title" );
            rootElement->addChild( window );

            auto window2 = Graphics::UserInterface::Widgets::Window::create( "test2", {}, "Error" );
            rootElement->addChild( window2 );

            window->getEventBundle().registerInputEvent( "mouse-down", []( Device::Input::Input::Metadata event ) {
              Log::getInstance().debug( "assert", "you clicked me" );
            } );
          }

          void GuiComponent::__teststyle() {
            Log::getInstance().info( "GuiComponent::__teststyle", "Remove this function and callbacks" );
          }

          std::shared_ptr< Graphics::UserInterface::Element > GuiComponent::captureMouseEvent( std::shared_ptr< Graphics::UserInterface::Element > element, Device::Input::Input::Metadata event ) {
            glm::uvec2 absolutePosition = element->getAbsolutePosition();
            glm::uvec4 allocation = element->getAllocation();
            glm::uvec4 region{ absolutePosition.x, absolutePosition.y, absolutePosition.x + allocation[ 2 ], absolutePosition.y + allocation[ 3 ] };

            if( event.mouseLocation.x >= region[ 0 ] && event.mouseLocation.y >= region[ 1 ] && event.mouseLocation.x <= region[ 2 ] && event.mouseLocation.y <= region[ 3 ] ) {
              std::vector< std::shared_ptr< Graphics::UserInterface::Element > > children = element->getSortedElements();

              for( auto it = children.rbegin(); it != children.rend(); ++it ) {
                if( captureMouseEvent( *it, event ) ) {
                  return *it;
                }
              }

              return element;
            }

            return nullptr;
          }

          void GuiComponent::mousePressed( Device::Input::Input::Metadata event ) {
            std::shared_ptr< Graphics::UserInterface::Element > captured = captureMouseEvent( rootElement, event );

            captured->getEventBundle().trigger( "mouse-down", event );
          }

          void GuiComponent::mouseReleased( Device::Input::Input::Metadata event ) {
          }

          Graphics::Vector::Renderer& GuiComponent::getVectorRenderer() {
            return vector;
          }

          Graphics::UserInterface::Style::StyleApplier& GuiComponent::getStyleManager() {
            return styleManager;
          }

          void GuiComponent::nextFrame() {
            glDisable( GL_CULL_FACE );
            glDisable( GL_DEPTH_TEST );

            guiShader.use( true );
            rootElement->draw();

            glEnable( GL_CULL_FACE );
            glEnable( GL_DEPTH_TEST );
          }

        }
      }
    }
  }
}
