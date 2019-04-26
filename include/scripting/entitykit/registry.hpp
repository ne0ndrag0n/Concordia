#ifndef ENTITY_REGISTRANT
#define ENTITY_REGISTRANT

#include "containers/reusableobjectvector.hpp"
#include "exceptions/genexc.hpp"
#include "scripting/entitykit/entity.hpp"
#include "scripting/entitykit/component.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <string>
#include <optional>
#include <variant>
#include <memory>
#include <map>

namespace BlueBear::Scripting::EntityKit {

  class Registry {
    sol::state* engineState;
    std::map< std::string, sol::table > components;
    std::map< std::string, std::vector< std::string > > entities;

    void submitLuaContributions( sol::state& lua );
    void registerComponent( const std::string& id, sol::table table );
    void registerEntity( const std::string& id, sol::table componentlist );

    bool entityRegistered( const std::string& id );
    bool componentRegistered( const std::string& id );

  public:
    EXCEPTION_TYPE( InvalidIDException, "Invalid ID!" );

    Registry();
    ~Registry();

    std::shared_ptr< Entity > createEntity( const std::string& registeredId, bool defaults = true );
    std::shared_ptr< Component > createComponent( const std::string& registeredId, sol::object initArgs );
  };

}

#endif
