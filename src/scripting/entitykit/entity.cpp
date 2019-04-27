#include "scripting/entitykit/entity.hpp"
#include "scripting/luakit/utility.hpp"
#include "tools/utility.hpp"
#include "log.hpp"

namespace BlueBear::Scripting::EntityKit {

  BasicEvent< void*, std::shared_ptr< Entity > > Entity::ENTITY_CLOSING;

  Entity::Entity( const std::string& entityId, const std::vector< std::shared_ptr< Component > >& components ) : entityId( entityId ), components( components ) {}

  Entity::Entity( const Entity& entity ) {
    entityId = entity.entityId;
    components = entity.components;

    associate( this );
  }

  Entity::~Entity() {
    // These objects should become useless to use by Lua, but not crash the game
    associate( nullptr );
  }

  const std::string& Entity::getId() const {
    return entityId;
  }

  void Entity::submitLuaContributions( sol::state& lua, sol::table types ) {
    types.new_usertype< EntityKit::Entity >( "Entity",
      "new", sol::no_constructor,
      "find_components", [ &lua ]( Entity& self, const std::string& componentId ) {
        return LuaKit::Utility::vectorToTable< Components::ComponentReturn >( lua, self.findComponents( componentId ) );
      },
      "attach_component", &Entity::attachComponent,
      "get_entity_id", &Entity::getId
    );
  }

  void Entity::associate( Entity* pointer ) {
    for( auto& component : components ) {
      component->attach( pointer );
    }
  }

  std::vector< Components::ComponentReturn > Entity::findComponents( const std::string& componentId ) {
    std::vector< Components::ComponentReturn > result;

    for( auto& component : components ) {
      if( component->getId() == componentId ) {
        result.push_back( Components::cast( component ) );
      }
    }

    return result;
  }

  void Entity::attachComponent( std::shared_ptr< Component > component ) {
    components.push_back( component );
    component->attach( this );
  }

  void Entity::close() {
    ENTITY_CLOSING.trigger( shared_from_this() );

    for( auto& component : components ) {
      component->drop();
    }
  }

}
