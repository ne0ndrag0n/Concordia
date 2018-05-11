#include "scripting/entitykit/registry.hpp"
#include "scripting/entitykit/entity.hpp"
#include "scripting/luakit/dynamicusertype.hpp"
#include "graphics/scenegraph/model.hpp"
#include "eventmanager.hpp"
#include "log.hpp"
#include <functional>

namespace BlueBear::Scripting::EntityKit {

  Registry::Registry() {
    eventManager.LUA_STATE_READY.listen( this, std::bind( &Registry::submitLuaContributions, this, std::placeholders::_1 ) );
  }

  Registry::~Registry() {
    eventManager.LUA_STATE_READY.stopListening( this );
  }

  void Registry::submitLuaContributions( sol::state& lua ) {
    if( lua[ "bluebear" ][ "entity" ] == sol::nil ) {
      lua[ "bluebear" ][ "entity" ] = lua.create_table();
    }

    sol::table entity = lua[ "bluebear" ][ "entity" ];

    entity.set_function( "register_component", &Registry::registerComponent, this );
    entity.set_function( "register_entity", &Registry::registerEntity, this );
    entity.set_function( "create_new_entity", &Registry::newEntity, this );

    sol::table types = lua[ "bluebear" ][ "entity" ][ "types" ] = lua.create_table();
    types.new_usertype< EntityKit::Component >( "Component",
      "new", sol::no_constructor,
      sol::meta_function::index, &LuaKit::DynamicUsertype::get,
      sol::meta_function::new_index, &LuaKit::DynamicUsertype::set,
      sol::meta_function::length, &LuaKit::DynamicUsertype::size,
      sol::base_classes, sol::bases< LuaKit::DynamicUsertype >()
    );
    types.new_usertype< EntityKit::Entity >( "Entity",
      "new", sol::no_constructor,
      "get_component", &Entity::getComponent
    );
  }

  void Registry::registerComponent( const std::string& id, sol::table table ) {
    components[ id ] = table;
  }

  void Registry::registerEntity( const std::string& id, const std::vector< std::string >& componentlist ) {
    // Verify all attached components exist and have been registered
    for( const std::string& component : componentlist ) {
      if( components.find( component ) == components.end() ) {
        Log::getInstance().error( "Registry::registerEntity", "Component " + component + " has not been registered!" );
        return;
      }
    }

    entities[ id ] = componentlist;
  }

  std::map< std::string, sol::object > Registry::tableToMap( sol::table table ) {
    std::map< std::string, sol::object > result;

    for( std::pair< sol::object, sol::object >& pair : table ) {
      if( pair.first.is< std::string >() ) {
        result[ pair.first.as< std::string >() ] = pair.second;
      }
    }

    return result;
  }

  Component Registry::newComponent( const std::string& id ) {
    auto it = components.find( id );
    if( it == components.end() ) {
      Log::getInstance().error( "Registry::newComponent", "This should not be happening!" );
    }

    return Component( tableToMap( it->second ) );
  }

  Entity Registry::newEntity( const std::string& id, std::map< std::string, sol::table > constructors ) {
    auto it = entities.find( id );
    if( it == entities.end() ) {
      Log::getInstance().error( "Registry::newEntity", "Component " + id + " has not been registered!" );
      throw InvalidIDException();
    }

    std::map< std::string, Component > components;
    for( const std::string& component : it->second ) {
      components[ component ] = newComponent( component );

      // Call its "init" function, should one exist
      sol::object init = components[ component ].get( "init" );
      if( init.is< sol::function >() ) {
        init.as< sol::function >()( init, constructors[ component ] );
      }
    }

    return Entity( components );
  }

}
