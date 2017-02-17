#include "graphics/gui/luaelement.hpp"
#include "graphics/display.hpp"
#include "tools/utility.hpp"
#include "eventmanager.hpp"
#include "log.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFGUI/SFGUI.hpp>
#include <string>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      /**
       * @static
       */
      int LuaElement::lua_onEvent( lua_State* L ) {
        LuaElement** userData = ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" );
        Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );

        // function "event" self

        if( lua_isstring( L, -2 ) ) {
          const char* eventType = lua_tostring( L, -2 );

          switch( Tools::Utility::hash( eventType ) ) {
            case Tools::Utility::hash( "click" ):
              {
                LuaElement& element = **userData;

                // Create the bucket for this widget if it doesn't exist, otherwise, return a new bucket
                Display::MainGameState::SignalMap& signalMap = self->masterSignalMap[ element.widget.get() ];

                // If there's a previous sfg::Widget::OnLeftClick registered for this widget instance, unref and kill it
                auto pair = signalMap.find( sfg::Widget::OnLeftClick );
                if( pair != signalMap.end() ) {
                  // Un-ref this element we're about to erase
                  luaL_unref( L, LUA_REGISTRYINDEX, pair->second );
                }

                // Track the master reference
                // Unref this if the pointer is ever removed!
                LuaReference masterReference = signalMap[ sfg::Widget::OnLeftClick ] = luaL_ref( L, LUA_REGISTRYINDEX ); // "event" self

                element.widget->GetSignal( sfg::Widget::OnLeftClick ).Connect( [ L, self, masterReference ]() {
                  // Create new "disposable" reference that will get ferried through
                  lua_rawgeti( L, LUA_REGISTRYINDEX, masterReference ); // object
                  self->instance.eventManager.UI_ACTION_EVENT.trigger( luaL_ref( L, LUA_REGISTRYINDEX ) ); // EMPTY
                } );

                lua_pushboolean( L, true ); // true "event" self
                return 1; // true
              }
              break;
            default:
              Log::getInstance().warn( "LuaElement::lua_onEvent", "Invalid event type specified: " + std::string( eventType ) );
          }
        } else {
          Log::getInstance().warn( "LuaElement::lua_onEvent", "Invalid event passed to on()" );
        }

        return 0;
      }

      /**
       * @static
       */
      int LuaElement::lua_getWidgetByID( lua_State* L ) {
        std::string selector;

        // called from bluebear.gui.get_widget_by_id
        if( lua_isstring( L, -1 ) ) {
          selector = std::string( lua_tostring( L, -1 ) );
        } else {
          Log::getInstance().warn( "LuaElement::lua_getWidgetByID", "Argument provided to get_widget_by_id was not a string." );
          return 0;
        }

        Display::MainGameState* self = ( Display::MainGameState* )lua_touserdata( L, lua_upvalueindex( 1 ) );
        std::shared_ptr< sfg::Widget > parentWidget;
        if( lua_gettop( L ) == 1 ) {
          parentWidget = self->gui.rootContainer;
        } else {
          std::shared_ptr< sfg::Widget >* widgetPtr = *( ( std::shared_ptr< sfg::Widget >** ) luaL_checkudata( L, 1, "bluebear_widget" ) );
          parentWidget = *widgetPtr;
        }

        // Holy moly is this going to get confusing quick
        // I also have the least amount of confidence in this code you could possibly imagine
        std::shared_ptr< sfg::Widget > widget = parentWidget->GetWidgetById( selector );
        if( widget ) {
          LuaElement** userData = ( LuaElement** )lua_newuserdata( L, sizeof( LuaElement* ) ); // userdata
          *userData = new LuaElement();

          ( **userData ).widget = widget;

          luaL_getmetatable( L, "bluebear_widget" ); // metatable userdata
          lua_setmetatable( L, -2 ); // userdata

          /*
          Log::getInstance().debug(
            "LuaElement::lua_getWidgetByID",
            "Creating a userdata for the found element " + selector + ", " + Tools::Utility::pointerToString( *userData )
          );
          */

          return 1;
        } else {
          std::string error = std::string( "Could not find widget with ID " ) + selector;
          return luaL_error( L, error.c_str() );
        }
      }

      /**
       * @static
       */
      int LuaElement::lua_gc( lua_State* L ) {
        LuaElement* widgetPtr = *( ( LuaElement** ) luaL_checkudata( L, 1, "bluebear_widget" ) );

        // Destroy the std::shared_ptr< sfg::Widget >. This should decrease the reference count by one.
        //Log::getInstance().debug( "LuaElement::lua_gc", "Deleting " + Tools::Utility::pointerToString( widgetPtr ) );
        delete widgetPtr;

        return 0;
      }

    }
  }
}
