#include "scripting/luakit/utility.hpp"
#include "scripting/luakit/library/json.hpp"
#include "tools/utility.hpp"
#include "configmanager.hpp"
#include <functional>

#include "log.hpp"

namespace BlueBear::Scripting::LuaKit {

  std::function< sol::table( sol::table, bool ) > Utility::copy;
  std::function< sol::state&() > Utility::getCurrentState;

  sol::table Utility::copyTable( sol::state& lua, sol::table original, bool deep ) {
    sol::table newTable = lua.create_table();

    for( std::pair< sol::object, sol::object >& pair : original ) {
      if( deep && pair.second.is< sol::table >() ) {
        newTable[ pair.first ] = copyTable( lua, pair.second, deep );
      } else {
        newTable[ pair.first ] = pair.second;
      }
    }

    return newTable;
  }

  void Utility::submitLuaContributions( sol::state& lua ) {
    if( lua[ "bluebear" ][ "util" ] == sol::nil ) {
      lua[ "bluebear" ][ "util" ] = lua.create_table();
    }

    sol::table util = lua[ "bluebear" ][ "util" ];
    util[ "json" ] = lua.script( Library::JSON );

    copy = [ &lua ]( sol::table original, bool deep ) { return copyTable( lua, original, deep ); };
    getCurrentState = [ &lua ]() -> sol::state& { return lua; };
    util[ "copy_table" ] = copy;

    util[ "get_fps" ] = []() -> double {
      return ConfigManager::getInstance().getIntValue( "fps_overview" );
    };

    util[ "split" ] = []( const std::string& string, const std::string& delim ) {
      std::vector< std::string > split;

      if( delim.size() ) {
        split = Tools::Utility::split( string, delim[ 0 ] );
      }

      return split;
    };

    util[ "sanitize_xml" ] = []( const std::string& input ) {
      return Tools::Utility::sanitizeXML( input );
    };
  }

}
