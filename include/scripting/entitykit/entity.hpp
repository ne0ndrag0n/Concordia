#ifndef ENTITYKIT_ENTITY
#define ENTITYKIT_ENTITY

#include "exceptions/genexc.hpp"
#include "scripting/entitykit/component.hpp"
#include <string>
#include <unordered_map>

namespace BlueBear::Scripting::EntityKit {

  class Entity {
    std::unordered_map< std::string, Component > components;

  public:
    EXCEPTION_TYPE( NotFoundException, "Object not found in entity" );

    Entity( std::unordered_map< std::string, Component > components );

    Component& getComponent( const std::string& componentId );
  };

}

#endif
