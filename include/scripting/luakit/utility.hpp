#ifndef LUAKIT_UTILITIES
#define LUAKIT_UTILITIES

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <functional>

namespace BlueBear::Scripting::LuaKit {

  struct Utility {
    static sol::table copyTable( sol::state& lua, sol::table original, bool deep );
    static void submitLuaContributions( sol::state& lua );

    static std::function< sol::table( sol::table, bool ) > copy;
  };

}

#endif
