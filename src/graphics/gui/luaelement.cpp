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

    }
  }
}
