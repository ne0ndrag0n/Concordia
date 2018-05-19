#ifndef LUAKIT_UTILITIES
#define LUAKIT_UTILITIES

#include "exceptions/genexc.hpp"
#include "log.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <functional>
#include <string>
#include <vector>

namespace BlueBear::Scripting::LuaKit {

  struct Utility {
    EXCEPTION_TYPE( InvalidTypeException, "Invalid type!" );

    template< typename... Args >
    static std::function< void( Args... ) > bagFunction( sol::function f ) {
      return [ f ]( Args... args ) {
        auto result = f( args... );

        if( !result.valid() ) {
          sol::error error = result;
          Log::getInstance().error( "Utility::bagFunction", error.what() );
        }
      };
    };

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

    template < typename T >
    static std::vector< T > tableToVector( sol::table table ) {
      std::vector< T > result;

      for( auto& pair : table ) {
        result.emplace_back( cast< T >( pair.second ) );
      }

      return result;
    };
  };

}

#endif
