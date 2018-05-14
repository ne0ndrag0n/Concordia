#ifndef COREENGINE_VECTOR_TYPES
#define COREENGINE_VECTOR_TYPES

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>

namespace BlueBear::Scripting {

  struct VectorTypes {
    static void submitLuaContributions( sol::table types );
  };

}

#endif
