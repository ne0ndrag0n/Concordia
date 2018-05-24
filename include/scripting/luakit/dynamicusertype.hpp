#ifndef DYNAMIC_LUA_USERTYPE
#define DYNAMIC_LUA_USERTYPE

#include "serializable.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <string>
#include <map>

namespace BlueBear::Scripting::LuaKit {

  class DynamicUsertype : public Serializable {
    std::map< std::string, sol::object > types;

  public:
    sol::object operator[]( const std::string& key );
    DynamicUsertype( const std::map< std::string, sol::object >& typeTable = {} );
    virtual ~DynamicUsertype() = default;

    void set( const std::string& key, sol::object object );
    sol::object get( const std::string& key );
    unsigned int size();

    static void submitLuaContributions( sol::table& types );

    Json::Value save() override;
    void load( const Json::Value& data ) override;
  };

}

#endif
