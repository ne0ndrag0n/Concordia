#ifndef ENTITYKIT_COMPONENT
#define ENTITYKIT_COMPONENT

#include "scripting/luakit/dynamicusertype.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <unordered_map>

namespace BlueBear::Scripting::EntityKit {
  class Entity;

  class Component : public LuaKit::DynamicUsertype {
    Entity* entity = nullptr;

  public:
    Component( const std::unordered_map< std::string, sol::object >& types = {} );

    void attach( Entity* entity );
  };

}

#endif
