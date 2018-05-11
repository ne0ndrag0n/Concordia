#include "scripting/entitykit/entity.hpp"

namespace BlueBear::Scripting::EntityKit {

  Entity::Entity( std::unordered_map< std::string, Component > components ) : components( components ) {}

  Component& Entity::getComponent( const std::string& componentId ) {
    auto it = components.find( componentId );
    if( it == components.end() ) {
      throw NotFoundException();
    }

    return it->second;
  }

}
