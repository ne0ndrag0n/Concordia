#include "scripting/luakit/eventbridge.hpp"
#include "scripting/engine.hpp"
#include "tools/ctvalidators.hpp"
#include "tools/utility.hpp"
#include "eventmanager.hpp"
// I guess we really gotta do this...
#include "log.hpp"
#include <functional>
#include <iostream>

namespace BlueBear {
  namespace Scripting {
    namespace LuaKit {

      EventBridge::EventBridge( lua_State* L, Engine& engine ) : engine( engine ), L( L ) {
        // TODO: Register one of each exposable EventManager type here
        // and in the callbacks, call the trigger method
        eventManager.MESSAGE_LOGGED.listen( this, std::bind( &EventBridge::fireEvents, this, std::ref( messageLogged ), std::placeholders::_1 ) );
      }

      /**
       * Fire off all events in references
       */
      void EventBridge::fireEvents( std::vector< LuaReference >& references, const std::string& logMessage ) {

        lua_getglobal( L, "bluebear" ); // bluebear
        Tools::Utility::getTableValue( L, "util" ); // bluebear.util bluebear

        for( LuaReference reference : references ) {
          Tools::Utility::getTableValue( L, "bind" ); // <bind> bluebear.util bluebear
          lua_rawgeti( L, LUA_REGISTRYINDEX, reference ); // <function> <bind> bluebear.util bluebear

          lua_pushstring( L, logMessage.c_str() ); // string <function> <bind> bluebear.util bluebear

          if( lua_pcall( L, 2, 1, 0 ) ) { // error bluebear.util bluebear
            // I really don't want to log anything here
            lua_pop( L, 3 ); // EMPTY
            return;
          } // <temp_function> bluebear.util bluebear

          engine.enqueue( luaL_ref( L, LUA_REGISTRYINDEX ) ); // bluebear.util bluebear
        }

        lua_pop( L, 2 ); // EMPTY
      }

      unsigned int EventBridge::enqueue( std::vector< LuaReference >& references, LuaReference masterReference ) {
        for( int i = 0; i != messageLogged.size(); i++ ) {
          if( messageLogged[ i ] == -1 ) {
            messageLogged[ i ] = masterReference;
            return i;
          }
        }

        messageLogged.push_back( masterReference );
        return messageLogged.size() - 1;
      }

      /**
       *
       */
      void EventBridge::invalidate( std::vector< LuaReference >& references, int index ) {
        LuaReference masterReference = references.at( index );

        references[ index ] = -1;

        luaL_unref( L, LUA_REGISTRYINDEX, masterReference );
      }

      /**
       *
       * STACK ARGS: <function>
       * Returns: EMPTY
       */
      unsigned int EventBridge::listen( const std::string& eventId ) {
        switch( Tools::Utility::hash( eventId.c_str() ) ) {
          case Tools::Utility::hash( "MESSAGE_LOGGED" ): {
            return enqueue( messageLogged, luaL_ref( L, LUA_REGISTRYINDEX ) ); // EMPTY
          }
        }

        throw InvalidEventException();
      }

      void EventBridge::unlisten( const std::string& eventId, int index ) {
        switch( Tools::Utility::hash( eventId.c_str() ) ) {
          case Tools::Utility::hash( "MESSAGE_LOGGED" ): {
            return invalidate( messageLogged, index );
          }
        }

        throw InvalidEventException();
      }

      int EventBridge::lua_listen( lua_State* L ) {
        VERIFY_FUNCTION_N( "EventBridge::lua_listen", "listen", 1 );
        VERIFY_STRING_N( "EventBridge::lua_listen", "listen", 2 );

        // <function> "string"

        EventBridge* self = ( EventBridge* )lua_touserdata( L, lua_upvalueindex( 1 ) );

        try {
          lua_pushnumber( L, self->listen( lua_tostring( L, -2 ) ) ); // 42 "string"
          return 1;
        } catch( InvalidEventException& e ) {
          return luaL_error( L, "EventBridge: Failed to register system event (invalid event type?)" );
        }
      }

      int EventBridge::lua_unlisten( lua_State* L ) {
        VERIFY_NUMBER_N( "EventBridge::lua_unlisten", "listen", 1 );
        VERIFY_STRING_N( "EventBridge::lua_unlisten", "listen", 2 );

        // 42 "string"

        EventBridge* self = ( EventBridge* )lua_touserdata( L, lua_upvalueindex( 1 ) );

        try {
          self->unlisten( lua_tostring( L, -2 ), lua_tointeger( L, -1 ) );
        } catch( std::exception& e ) {
          return luaL_error( L, "EventBridge: Failed to unregister system event (invalid event type or invalid handle given?)" );
        }

        return 0;
      }

    }
  }
}
