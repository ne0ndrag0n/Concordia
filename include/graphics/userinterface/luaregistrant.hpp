#ifndef LUA_REGISTRANT
#define LUA_REGISTRANT

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>

namespace BlueBear::Graphics::UserInterface {

  struct LuaRegistrant {
    static void registerWidgets( sol::state& lua );
  };

}

#endif
