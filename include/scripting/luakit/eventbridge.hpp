#ifndef EVENTBRIDGE
#define EVENTBRIDGE

#include "bbtypes.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <vector>
#include <string>
#include <exception>

namespace BlueBear {
  namespace Scripting {
    class Engine;

    namespace LuaKit {

      class EventBridge {
        Scripting::Engine& engine;
        lua_State* L;

        std::vector< LuaReference > messageLogged;

        void fireEvents( std::vector< LuaReference >& references, const std::string& logMessage );

        unsigned int enqueue( std::vector< LuaReference >& references, LuaReference masterReference );
        void invalidate( std::vector< LuaReference >& references, int index );

        struct InvalidEventException : public std::exception {

          const char* what() const throw() {
            return "Event type not valid!";
          }

        };

      public:
        EventBridge( lua_State* L, Scripting::Engine& engine );

        unsigned int listen( const std::string& eventId );
        void unlisten( const std::string& eventId, int index );

        static int lua_listen( lua_State* L );
        static int lua_unlisten( lua_State* L );
      };

    }
  }
}

#endif
