#ifndef ENTITY_REGISTRANT
#define ENTITY_REGISTRANT

#include "containers/reusableobjectvector.hpp"
#include "exceptions/genexc.hpp"
#include "scripting/entitykit/entity.hpp"
#include "scripting/entitykit/component.hpp"
#include "serializable.hpp"
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

  class Registry : public Serializable {
    std::map< std::string, sol::table > components;
    std::map< std::string, std::vector< std::string > > entities;

    Containers::ReusableObjectVector< std::shared_ptr< Entity > > instances;

    std::map< std::string, sol::object > tableToMap( sol::table table );
    void submitLuaContributions( sol::state& lua );
    void registerComponent( const std::string& id, sol::table table );
    void registerEntity( const std::string& id, sol::table componentlist );

    bool entityRegistered( const std::string& id );
    bool componentRegistered( const std::string& id );

    std::shared_ptr< Component > newComponent( const std::string& id );
    std::shared_ptr< Entity > newEntity( const std::string& id, std::variant< sol::nil_t, sol::table > constructors );

    void forget( std::shared_ptr< Entity > entity );

  public:
    EXCEPTION_TYPE( InvalidIDException, "Invalid ID!" );

    Json::Value save() override;
    void load( const Json::Value& data ) override;

    Registry();
    ~Registry();
  };

}

#endif
