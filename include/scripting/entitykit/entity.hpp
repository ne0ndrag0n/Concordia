#ifndef ENTITYKIT_ENTITY
#define ENTITYKIT_ENTITY

#include "exceptions/genexc.hpp"
#include "scripting/entitykit/component.hpp"
#include "scripting/entitykit/components/componentcast.hpp"
#include "eventmanager.hpp"
#include "serializable.hpp"
#include <sol.hpp>
#include <string>
#include <memory>
#include <map>
#include <vector>

namespace BlueBear::Scripting::EntityKit {

  class Entity : public std::enable_shared_from_this< Entity > {
    std::string entityId;
    std::vector< std::shared_ptr< Component > > components;

    void associate( Entity* pointer );

  public:
    static BasicEvent< void*, std::shared_ptr< Entity > > ENTITY_CLOSING;
    EXCEPTION_TYPE( NotFoundException, "Object not found in entity" );

    Entity( const std::string& entityId, const std::vector< std::shared_ptr< Component > >& components );
    Entity( const Entity& entity );
    ~Entity();

    const std::string& getId() const;

    void attachComponent( std::shared_ptr< Component > component );

    void close();

    static void submitLuaContributions( sol::state& lua, sol::table types );

    std::vector< Components::ComponentReturn > findComponents( const std::string& componentId );
  };

}

#endif
