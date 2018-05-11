#include "device/display/adapter/component/guicomponent.hpp"
#include "device/display/display.hpp"
#include "device/input/input.hpp"
#include "graphics/userinterface/element.hpp"
#include "graphics/userinterface/widgets/layout.hpp"
#include "graphics/userinterface/propertylist.hpp"
#include "graphics/userinterface/drawable.hpp"
#include "graphics/userinterface/draghelper.hpp"
#include "graphics/userinterface/querier.hpp"
#include "graphics/userinterface/luaregistrant.hpp"
#include "tools/utility.hpp"
#include "configmanager.hpp"
#include "eventmanager.hpp"
#include "log.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <algorithm>

#include <glm/gtx/string_cast.hpp>
#include "graphics/userinterface/event/eventbundle.hpp"
#include "graphics/userinterface/style/style.hpp"
#include "graphics/userinterface/xmlloader.hpp"

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

              currentFocus = rootElement;

              styleManager.applyStyles( {
                "system/ui/system.style"
              } );

              eventManager.LUA_STATE_READY.listen( this, std::bind( &GuiComponent::submitLuaContributions, this, std::placeholders::_1 ) );
            }

          GuiComponent::~GuiComponent() {
            eventManager.LUA_STATE_READY.stopListening( this );
          }

          void GuiComponent::submitLuaContributions( sol::state& lua ) {
            sol::table gui = lua.create_table();
            gui.set_function( "load_xml", [ & ]( const std::string& path ) {
              Graphics::UserInterface::XMLLoader loader( path );
              std::vector< std::shared_ptr< Graphics::UserInterface::Element > > elements = loader.getElements();
              for( std::shared_ptr< Graphics::UserInterface::Element > element : elements ) {
                rootElement->addChild( element );
              }
            } );

            lua[ "bluebear" ][ "gui" ] = gui;
            Graphics::UserInterface::LuaRegistrant::registerWidgets( lua );
          }

          // TODO: remove TEST code
          void GuiComponent::__testadd() {
            Graphics::UserInterface::XMLLoader loader( "system/ui/example.xml" );
            std::vector< std::shared_ptr< Graphics::UserInterface::Element > > elements = loader.getElements();
            for( std::shared_ptr< Graphics::UserInterface::Element > element : elements ) {
              rootElement->addChild( element );
            }
          }

          void GuiComponent::__teststyle() {
            Log::getInstance().info( "GuiComponent::__teststyle", "Remove this function and callbacks" );
          }

          void GuiComponent::setupBlockingGlobalEvent( const std::string& eventId, std::function< void( Device::Input::Metadata ) > callback ) {
            blockingGlobalEvents[ eventId ] = [ callback ]( Device::Input::Metadata event ) {
              callback( event );

              event.cancelAll();
            };
          }

          void GuiComponent::unregisterBlockingGlobalEvent( const std::string& eventId ) {
            blockingGlobalEvents.erase( eventId );
          }

          void GuiComponent::fireFocusEvent( std::shared_ptr< Graphics::UserInterface::Element > selected, Device::Input::Metadata event ) {
            if( selected != currentFocus ) {
              currentFocus->getEventBundle().trigger( "blur", event, false );
              selected->getEventBundle().trigger( "focus", event, false );

              currentFocus = selected;
            }
          }

          void GuiComponent::fireInOutEvents( std::shared_ptr< Graphics::UserInterface::Element > selected, Device::Input::Metadata event ) {
            std::set< std::shared_ptr< Graphics::UserInterface::Element > > currentMove;
            std::set< std::shared_ptr< Graphics::UserInterface::Element > > symmetricDifference;

            // Unroll
            while( selected ) {
              currentMove.insert( selected );
              selected = selected->getParent();
            }

            std::set_symmetric_difference( currentMove.begin(), currentMove.end(), previousMove.begin(), previousMove.end(), std::inserter( symmetricDifference, symmetricDifference.end() ) );

            for( std::shared_ptr< Graphics::UserInterface::Element > target : symmetricDifference ) {
              if( previousMove.count( target ) ) {
                // target is in the old list, that's a mouse-out event
                target->getEventBundle().trigger( "mouse-out", event, false );
              } else {
                // target is in the new list, that's a mouse-in event
                target->getEventBundle().trigger( "mouse-in", event, false );
              }
            }

            previousMove = currentMove;
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
            // Swallow an event if it's a blocking global
            auto it = blockingGlobalEvents.find( "mouse-down" );
            if( it != blockingGlobalEvents.end() ) {
              it->second( event );
              return;
            }

            // Swallow the event if a drag is in progress (no wheel or right click while dragging)
            if( !currentDrag ) {
              std::shared_ptr< Graphics::UserInterface::Element > captured = captureMouseEvent( rootElement, event );

              if( captured ) {
                fireFocusEvent( captured, event );
                captured->getEventBundle().trigger( "mouse-down", event );
              }
            }
          }

          void GuiComponent::mouseMoved( Device::Input::Metadata event ) {
            // Swallow an event if it's a blocking global
            auto it = blockingGlobalEvents.find( "mouse-moved" );
            if( it != blockingGlobalEvents.end() ) {
              it->second( event );
              return;
            }

            // Pass the event if a drag is in progress
            if( currentDrag ) {
              return currentDrag->update( event );
            } else {
              std::shared_ptr< Graphics::UserInterface::Element > captured = captureMouseEvent( rootElement, event );

              if( captured ) {
                fireInOutEvents( captured, event );
                captured->getEventBundle().trigger( "mouse-moved", event );
              }
            }
          }

          void GuiComponent::mouseReleased( Device::Input::Metadata event ) {
            // Swallow an event if it's a blocking global
            auto it = blockingGlobalEvents.find( "mouse-up" );
            if( it != blockingGlobalEvents.end() ) {
              it->second( event );
              return;
            }

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

          void GuiComponent::keyPressed( Device::Input::Metadata event ) {
            if( currentFocus ) {
              currentFocus->getEventBundle().trigger( "key-down", event );
            }
          }

          void GuiComponent::keyReleased( Device::Input::Metadata event ) {
            if( currentFocus ) {
              currentFocus->getEventBundle().trigger( "key-up", event );
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
            rootElement->walk( []( Graphics::UserInterface::Element& element ) {
              element.getPropertyList().updateAnimation();
            } );
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
