#ifndef EVENTBRIDGE
#define EVENTBRIDGE

#include "bbtypes.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <vector>

namespace BlueBear {
  namespace Scripting {
    class Engine;

    namespace LuaKit {

      class EventBridge {
        Scripting::Engine& engine;

        std::vector< LuaReference > messageLogged;

        void fireEvents( std::vector< LuaReference >& reference );

      public:
        EventBridge( Scripting::Engine& engine );

        int listen( const std::string& eventId, LuaReference masterReference );
        void unlisten( const std::string& eventId, int index );

        static int lua_listen( lua_State* L );
        static int lua_unlisten( lua_State* L );
      };

    }
  }
}

#endif
