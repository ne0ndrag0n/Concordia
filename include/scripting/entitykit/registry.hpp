#ifndef ENTITY_REGISTRANT
#define ENTITY_REGISTRANT

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <string>
#include <unordered_map>

namespace BlueBear::Scripting::EntityKit {

  class Registry {
    std::unordered_map< std::string, sol::object > components;
    std::unordered_map< std::string, sol::object > entities;

    void submitLuaContributions( sol::state& lua );
    void registerComponent( const std::string& id, sol::table table );
    void registerEntity( const std::string& id, sol::table table );

  public:
    Registry();
    ~Registry();
  };

}

#endif
