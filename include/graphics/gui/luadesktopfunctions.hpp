#ifndef LUADESKTOPFUNCS
#define LUADESKTOPFUNCS

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

namespace BlueBear {
  namespace Graphics {
    namespace GUI {

      class LuaDesktopFunctions {
      public:
        static int lua_addXMLFromPath( lua_State* L );
        static int lua_add( lua_State* L );
        static int lua_createWidget( lua_State* L );
        static int lua_removeWidget( lua_State* L );
        static int lua_findById( lua_State* L );
        static int lua_findByClass( lua_State* L );

      };

    }
  }
}

#endif
