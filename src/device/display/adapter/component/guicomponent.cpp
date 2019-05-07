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
#include "graphics/userinterface/xmlloader.hpp"
#include "graphics/userinterface/style/ast/selectorquery.hpp"
#include "graphics/utilities/shader_manager.hpp"
#include "scripting/luakit/utility.hpp"
#include "tools/utility.hpp"
#include "configmanager.hpp"
#include "eventmanager.hpp"
#include "log.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <algorithm>

namespace BlueBear {
  namespace Device {
    namespace Display {
      namespace Adapter {
        namespace Component {

          GuiComponent::GuiComponent( Device::Display::Display& display, Graphics::Utilities::ShaderManager& shaderManager ) :
            Adapter::Adapter( display ),
            vector( display ),
            guiShader( shaderManager.getShader( "system/shaders/gui/vertex.glsl", "system/shaders/gui/fragment.glsl" ) ),
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
            gui.set_function( "load_xml", sol::overload(
              [ & ]( const std::string& xmlPath ) {
                return Scripting::LuaKit::Utility::vectorToTable( lua, addElementsFromXML( xmlPath ) );
              },
              [ & ]( const std::string& xmlPath, bool asString ) {
                return Scripting::LuaKit::Utility::vectorToTable( lua, addElementsFromXML( xmlPath, asString ) );
              }
            ) );
            gui.set_function( "load_stylesheet", [ & ]( sol::table table ) {
              loadStylesheets( Scripting::LuaKit::Utility::tableToVector< std::string >( table ) );
            } );
            gui.set_function( "get_elements", [ & ]( sol::table queries ) {
              return Scripting::LuaKit::Utility::vectorToTable( lua, query( queries ) );
            } );

            lua[ "bluebear" ][ "gui" ] = gui;
            Graphics::UserInterface::LuaRegistrant::registerWidgets( lua );

            gui[ "root_element" ] = rootElement;
          }

          void GuiComponent::registerEvents( Device::Input::Input& inputManager ) {
            inputManager.registerInputEvent(
              sf::Event::KeyPressed,
              std::bind( &Device::Display::Adapter::Component::GuiComponent::keyPressed, this, std::placeholders::_1 )
            );

            inputManager.registerInputEvent(
              sf::Event::KeyReleased,
              std::bind( &Device::Display::Adapter::Component::GuiComponent::keyReleased, this, std::placeholders::_1 )
            );

            inputManager.registerInputEvent(
              sf::Event::MouseButtonPressed,
              std::bind( &Device::Display::Adapter::Component::GuiComponent::mousePressed, this, std::placeholders::_1 )
            );

            inputManager.registerInputEvent(
              sf::Event::MouseButtonReleased,
              std::bind( &Device::Display::Adapter::Component::GuiComponent::mouseReleased, this, std::placeholders::_1 )
            );

            inputManager.registerInputEvent(
              sf::Event::MouseMoved,
              std::bind( &Device::Display::Adapter::Component::GuiComponent::mouseMoved, this, std::placeholders::_1 )
            );
          }

          void GuiComponent::loadStylesheets( const std::vector< std::string >& paths ) {
            styleManager.applyStyles( paths );
          }

          std::vector< std::shared_ptr< Graphics::UserInterface::Element > > GuiComponent::addElementsFromXML( const std::string& xmlPath, bool file ) {
            Graphics::UserInterface::XMLLoader loader( xmlPath, file );
            return loader.getElements();
          }

          void GuiComponent::addElement( std::shared_ptr< Graphics::UserInterface::Element > element ) {
            rootElement->addChild( element );
          }

          std::vector< std::shared_ptr< Graphics::UserInterface::Element > > GuiComponent::query( sol::table queries ) {
            std::vector< std::shared_ptr< Graphics::UserInterface::Element > > results;
            Graphics::UserInterface::Querier querier( rootElement );

            std::vector< Graphics::UserInterface::Style::AST::SelectorQuery > totalQuery;
            for( auto& pair : queries ) {
              sol::table subtable = Scripting::LuaKit::Utility::cast< sol::table >( pair.second );
              totalQuery.emplace_back( Graphics::UserInterface::Style::AST::SelectorQuery{
                ( subtable[ "tag" ] == sol::nil ) ? "" : Scripting::LuaKit::Utility::cast< std::string >( subtable[ "tag" ] ),
                ( subtable[ "id" ] == sol::nil ) ? "" : Scripting::LuaKit::Utility::cast< std::string >( subtable[ "id" ] ),
                ( subtable[ "classes" ] == sol::nil ) ? std::vector< std::string >{} : ( [ &subtable ]() {
                  sol::table classList = Scripting::LuaKit::Utility::cast< sol::table >( subtable[ "classes" ] );
                  std::vector< std::string > result;

                  for( auto& pair : classList ) {
                    result.push_back( Scripting::LuaKit::Utility::cast< std::string >( pair.second ) );
                  }

                  return result;
                } )(),
                ( subtable[ "all" ] == true )
              } );
            }

            return querier.get( totalQuery );
          }

          void GuiComponent::removeElement( std::shared_ptr< Graphics::UserInterface::Element > element ) {
            rootElement->remove( { element } );
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
            // Cancel event for rest of tick if it is captured anywhere in the GUI tree, besides rootElement
            std::shared_ptr< Graphics::UserInterface::Element > captured = captureMouseEvent( rootElement, event );
            if( captured && captured != rootElement ) {
              event.cancelAll();
            }

            // Swallow an event if it's a blocking global
            auto it = blockingGlobalEvents.find( "mouse-down" );
            if( it != blockingGlobalEvents.end() ) {
              it->second( event );
              return;
            }

            // Swallow the event if a drag is in progress (no wheel or right click while dragging)
            if( !currentDrag ) {
              if( captured ) {
                fireFocusEvent( captured, event );
                captured->getEventBundle().trigger( "mouse-down", event );
                GUI_OBJECT_MOUSE_DOWN.trigger( captured );
              }
            }
          }

          void GuiComponent::mouseMoved( Device::Input::Metadata event ) {
            // Cancel event for rest of tick if it is captured anywhere in the GUI tree, besides rootElement
            std::shared_ptr< Graphics::UserInterface::Element > captured = captureMouseEvent( rootElement, event );
            if( captured && captured != rootElement ) {
              event.cancelAll();
            }

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
              if( captured ) {
                fireInOutEvents( captured, event );
                captured->getEventBundle().trigger( "mouse-moved", event );
              }
            }
          }

          void GuiComponent::mouseReleased( Device::Input::Metadata event ) {
            // Cancel event for rest of tick if it is captured anywhere in the GUI tree, besides rootElement
            std::shared_ptr< Graphics::UserInterface::Element > captured = captureMouseEvent( rootElement, event );
            if( captured && captured != rootElement ) {
              event.cancelAll();
            }

            // Swallow an event if it's a blocking global
            auto it = blockingGlobalEvents.find( "mouse-up" );
            if( it != blockingGlobalEvents.end() ) {
              it->second( event );
              return;
            }

            // Destroy the drag helper and swallow the event
            if( currentDrag ) {
              currentDrag->commit();
              currentDrag = nullptr;
            } else {
              if( captured ) {
                captured->getEventBundle().trigger( "mouse-up", event );
                GUI_OBJECT_MOUSE_UP.trigger( captured );
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

            guiShader->use( true );
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
