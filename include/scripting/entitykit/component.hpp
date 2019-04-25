#ifndef ENTITYKIT_COMPONENT
#define ENTITYKIT_COMPONENT

#include "exceptions/genexc.hpp"
#include "serializable.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>

namespace BlueBear::Scripting::EntityKit {
  class Entity;

  class Component : public Serializable {
    std::string componentId;
    Entity* entity = nullptr;

    Entity& getEntity();

  public:
    EXCEPTION_TYPE( EntityDestroyedException, "Parent entity has been destroyed" );

    Component( const std::string& componentId );

    Json::Value save() override;
    void load( const Json::Value& data ) override;

    static void submitLuaContributions( sol::state& lua, sol::table types );

    void attach( Entity* entity );

    const std::string& getId() const;

    virtual void init( sol::object object );
    virtual void drop();
  };

}

#endif
