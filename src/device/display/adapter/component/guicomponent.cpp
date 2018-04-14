#include "device/display/adapter/component/guicomponent.hpp"
#include "device/display/display.hpp"
#include "device/input/input.hpp"
#include "graphics/userinterface/element.hpp"
#include "graphics/userinterface/widgets/layout.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/drawable.hpp"
#include "graphics/userinterface/draghelper.hpp"
#include "tools/utility.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <glm/gtx/string_cast.hpp>
#include "graphics/userinterface/event/eventbundle.hpp"
#include "graphics/userinterface/widgets/window.hpp"
#include "graphics/userinterface/widgets/text.hpp"
#include <glm/gtx/string_cast.hpp>

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

          GuiComponent::~GuiComponent() = default;

          // TODO: remove TEST code
          void GuiComponent::__testadd() {
            auto window = Graphics::UserInterface::Widgets::Window::create( "test", {}, "Window Title" );
            rootElement->addChild( window );

            auto window2 = Graphics::UserInterface::Widgets::Window::create( "test2", {}, "Error" );
            rootElement->addChild( window2 );

            auto text = Graphics::UserInterface::Widgets::Text::create( "", {}, "fart" );
            window2->addChild( text );

            text->getEventBundle().registerInputEvent( "mouse-down", [ & ]( Device::Input::Metadata event ) {
              Log::getInstance().debug( "assert", "fart" );
            } );
          }

          void GuiComponent::__teststyle() {
            Log::getInstance().info( "GuiComponent::__teststyle", "Remove this function and callbacks" );
          }

          std::shared_ptr< Graphics::UserInterface::Element > GuiComponent::captureMouseEvent( std::shared_ptr< Graphics::UserInterface::Element > element, Device::Input::Metadata event ) {
            glm::ivec2 absolutePosition = element->getAbsolutePosition();
            glm::ivec4 allocation = element->getAllocation();

            if( Tools::Utility::intersect( event.mouseLocation, { absolutePosition.x, absolutePosition.y, absolutePosition.x + allocation[ 2 ], absolutePosition.y + allocation[ 3 ] } ) ) {
              std::vector< std::shared_ptr< Graphics::UserInterface::Element > > children = element->getChildren();

              for( auto it = children.rbegin(); it != children.rend(); ++it ) {
                if( std::shared_ptr< Graphics::UserInterface::Element > result = captureMouseEvent( *it, event ) ) {
                  return result;
                }
              }

              return element;
            }

            return nullptr;
          }

          void GuiComponent::mousePressed( Device::Input::Metadata event ) {
            // Swallow the event if a drag is in progress (no wheel or right click while dragging)
            if( !currentDrag ) {
              std::shared_ptr< Graphics::UserInterface::Element > captured = captureMouseEvent( rootElement, event );

              if( captured ) {
                captured->getEventBundle().trigger( "mouse-down", event );
              }
            }
          }

          void GuiComponent::mouseMoved( Device::Input::Metadata event ) {
            // Pass the event if a drag is in progress
            if( currentDrag ) {
              return currentDrag->update( event );
            } else {
              std::shared_ptr< Graphics::UserInterface::Element > captured = captureMouseEvent( rootElement, event );

              if( captured ) {
                captured->getEventBundle().trigger( "mouse-moved", event );
              }
            }
          }

          void GuiComponent::mouseReleased( Device::Input::Metadata event ) {
            // Destroy the drag helper and swallow the event
            if( currentDrag ) {
              currentDrag = nullptr;
            } else {
              std::shared_ptr< Graphics::UserInterface::Element > captured = captureMouseEvent( rootElement, event );

              if( captured ) {
                captured->getEventBundle().trigger( "mouse-up", event );
              }
            }
          }

          Graphics::Vector::Renderer& GuiComponent::getVectorRenderer() {
            return vector;
          }

          Graphics::UserInterface::Style::StyleApplier& GuiComponent::getStyleManager() {
            return styleManager;
          }

          void GuiComponent::startDrag( std::shared_ptr< Graphics::UserInterface::Element > target, const glm::ivec2& offset ) {
            currentDrag = std::make_unique< Graphics::UserInterface::DragHelper >( target, offset );
          }

          void GuiComponent::nextFrame() {
            glDisable( GL_CULL_FACE );
            glDisable( GL_DEPTH_TEST );

            glEnable( GL_SCISSOR_TEST );

            guiShader.use( true );
            rootElement->draw();

            glEnable( GL_CULL_FACE );
            glEnable( GL_DEPTH_TEST );

            glDisable( GL_SCISSOR_TEST );
          }

        }
      }
    }
  }
}
