#include "scripting/luakit/eventbridge.hpp"
#include "scripting/engine.hpp"
#include "eventmanager.hpp"

namespace BlueBear {
  namespace Scripting {
    namespace LuaKit {

      EventBridge::EventBridge( Engine& engine ) : engine( engine ) {
        // TODO: Register one of each exposable EventManager type here
        // and in the callbacks, call the trigger method
      }

      void EventBridge::fireEvents( std::vector< LuaReference >& reference ) {

      }

      int EventBridge::listen( const std::string& eventId, LuaReference masterReference ) {
        // TODO
        return 0;
      }

      void EventBridge::unlisten( const std::string& eventId, int index ) {
        // TODO
      }

      int EventBridge::lua_listen( lua_State* L ) {
        return 0;
      }

      int EventBridge::lua_unlisten( lua_State* L ) {
        return 0;
      }

    }
  }
}
