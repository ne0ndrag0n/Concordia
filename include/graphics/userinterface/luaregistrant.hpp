#ifndef LUA_REGISTRANT
#define LUA_REGISTRANT

#include "exceptions/genexc.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>

namespace BlueBear::Graphics::UserInterface {

  struct LuaRegistrant {
    EXCEPTION_TYPE( InvalidTypeException, "Invalid type!" );
    static void registerWidgets( sol::state& lua );
  };

}

#endif
