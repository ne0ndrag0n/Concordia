#ifndef ENTITYKIT_ENTITY
#define ENTITYKIT_ENTITY

#include "exceptions/genexc.hpp"
#include "scripting/entitykit/component.hpp"
#include <string>
#include <map>

namespace BlueBear::Scripting::EntityKit {

  class Entity {
    std::map< std::string, Component > components;

    void associate();

  public:
    EXCEPTION_TYPE( NotFoundException, "Object not found in entity" );

    Entity( std::map< std::string, Component > components );
    Entity( const Entity& entity );

    Component& getComponent( const std::string& componentId );
  };

}

#endif
