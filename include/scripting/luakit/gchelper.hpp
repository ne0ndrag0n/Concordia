#ifndef GCHELPER
#define GCHELPER

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <string>

namespace BlueBear {
  namespace Scripting {
    namespace LuaKit {

      /**
       * Track an element that must not be allowed to go out of scope
       */
      class GcHelper {
        static void track( lua_State* L, const std::string& tag );
        static void untrack( lua_State* L, const std::string& tag );
        static void initialize( lua_State* L );
      };
    }
  }
}


#endif
