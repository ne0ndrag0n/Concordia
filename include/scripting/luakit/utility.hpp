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
#include <set>

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
    static std::function< sol::state&() > getCurrentState;

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

    template< typename T >
    static std::set< T > tableToSet( sol::table table ) {
      std::set< T > result;

      for( const auto& pair : table ) {
        result.insert( cast< T >( pair.second ) );
      }

      return result;
    };

    template< typename T >
    static sol::table vectorToTable( sol::state& lua, const std::vector< T >& vector ) {
      sol::table result = lua.create_table();

      for( const auto& element : vector ) {
        result.add( element );
      }

      return result;
    }
  };

}

#endif
