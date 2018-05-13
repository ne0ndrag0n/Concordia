#ifndef ENTITYKIT_COMPONENT
#define ENTITYKIT_COMPONENT

#include "exceptions/genexc.hpp"
#include "scripting/luakit/dynamicusertype.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <map>

namespace BlueBear::Scripting::EntityKit {
  class Entity;

  class Component : public LuaKit::DynamicUsertype {
    Entity* entity = nullptr;

    Entity& getEntity();

  public:
    EXCEPTION_TYPE( EntityDestroyedException, "Parent entity has been destroyed" );

    Component( const std::map< std::string, sol::object >& types = {} );
    static void submitLuaContributions( sol::state& lua, sol::table types );
    void attach( Entity* entity );

    virtual void init( sol::object object );
  };

}

#endif
