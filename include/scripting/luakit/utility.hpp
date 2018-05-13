#ifndef LUAKIT_UTILITIES
#define LUAKIT_UTILITIES

#include "exceptions/genexc.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <functional>

namespace BlueBear::Scripting::LuaKit {

  struct Utility {
    EXCEPTION_TYPE( InvalidTypeException, "Invalid type!" );

    static sol::table copyTable( sol::state& lua, sol::table original, bool deep );
    static void submitLuaContributions( sol::state& lua );

    static std::function< sol::table( sol::table, bool ) > copy;

    template< typename T >
    static T cast( sol::object o ) {
      if( o.is< T >() ) {
        return o.as< T >();
      }

      throw InvalidTypeException();
    };
  };

}

#endif
