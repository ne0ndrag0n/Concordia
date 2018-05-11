#ifndef DYNAMIC_LUA_USERTYPE
#define DYNAMIC_LUA_USERTYPE

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <string>
#include <map>

namespace BlueBear::Scripting::LuaKit {

  class DynamicUsertype {
    std::map< std::string, sol::object > types;

  public:
    DynamicUsertype( const std::map< std::string, sol::object >& types = {} );

    void set( const std::string& key, sol::object object );
    sol::object get( const std::string& key );
    unsigned int size();

    static void submitLuaContributions( sol::table& types );
  };

}

#endif
