#include "scripting/entitykit/entity.hpp"

namespace BlueBear::Scripting::EntityKit {

  Entity::Entity( std::map< std::string, Component > components ) : components( components ) {
    associate();
  }

  Entity::Entity( const Entity& entity ) {
    components = entity.components;

    associate();
  }

  void Entity::associate() {
    for( auto& pair : components ) {
      pair.second.attach( this );
    }
  }

  Component& Entity::getComponent( const std::string& componentId ) {
    auto it = components.find( componentId );
    if( it == components.end() ) {
      throw NotFoundException();
    }

    return it->second;
  }

}
