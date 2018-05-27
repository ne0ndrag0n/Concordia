#ifndef ENTITYKIT_ENTITY
#define ENTITYKIT_ENTITY

#include "exceptions/genexc.hpp"
#include "scripting/entitykit/component.hpp"
#include "scripting/entitykit/components/componentcast.hpp"
#include "serializable.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <string>
#include <memory>
#include <map>

namespace BlueBear::Scripting::EntityKit {

  class Entity : public Serializable {
    std::map< std::string, std::shared_ptr< Component > > components;

    void associate( Entity* pointer );

  public:
    EXCEPTION_TYPE( NotFoundException, "Object not found in entity" );

    Entity( std::map< std::string, std::shared_ptr< Component > > components );
    Entity( const Entity& entity );
    ~Entity();

    Json::Value save() override;
    void load( const Json::Value& data ) override;

    void close();

    static void submitLuaContributions( sol::state& lua, sol::table types );

    Components::ComponentReturn getComponent( const std::string& componentId );
  };

}

#endif
