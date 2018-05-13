#include "scripting/entitykit/entity.hpp"

namespace BlueBear::Scripting::EntityKit {

  Entity::Entity( std::map< std::string, std::shared_ptr< Component > > components ) : components( components ) {
    associate();
  }

  Entity::Entity( const Entity& entity ) {
    components = entity.components;

    associate();
  }

  Entity::~Entity() {
    // These objects should become useless to use by Lua, but not crash the game
    for( auto& pair : components ) {
      pair.second->attach( nullptr );
    }
  }

  void Entity::submitLuaContributions( sol::state& lua, sol::table types ) {
    types.new_usertype< EntityKit::Entity >( "Entity",
      "new", sol::no_constructor,
      "get_component", &Entity::getComponent
    );
  }

  void Entity::associate() {
    for( auto& pair : components ) {
      pair.second->attach( this );
    }
  }

  Components::ComponentReturn Entity::getComponent( const std::string& componentId ) {
    auto it = components.find( componentId );
    if( it == components.end() ) {
      throw NotFoundException();
    }

    return Components::cast( it->second );
  }

}
