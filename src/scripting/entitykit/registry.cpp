#include "scripting/entitykit/registry.hpp"
#include "scripting/entitykit/entity.hpp"
#include "scripting/entitykit/luacomponent.hpp"
#include "scripting/entitykit/components/modelmanager.hpp"
#include "scripting/luakit/dynamicusertype.hpp"
#include "graphics/scenegraph/model.hpp"
#include "containers/visitor.hpp"
#include "tools/utility.hpp"
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

  Json::Value Registry::save() {
    return {};
  }

  void Registry::load( const Json::Value& data ) {
    if( data != Json::Value::null ) {
      const Json::Value& entities = data[ "entities" ];
      if( entities.isArray() ) {
        for( Json::Value::const_iterator it = entities.begin(); it != entities.end(); ++it ) {
          // TODO: Unserialize the new format:
          /*
            {
              "id": "game.entity.plant",
              "components": [
                {
                  "id": "system.component.model_manager",
                  "data": {
                    "armaturebox": "dev/box/armaturebox.fbx"
                  }
                }
              ]
            }
          */

        }
      }
    }
  }

  void Registry::submitLuaContributions( sol::state& lua ) {
    if( lua[ "bluebear" ][ "entity" ] == sol::nil ) {
      lua[ "bluebear" ][ "entity" ] = lua.create_table();
    }

    sol::table entity = lua[ "bluebear" ][ "entity" ];

    entity.set_function( "register_component", &Registry::registerComponent, this );
    entity.set_function( "register_entity", &Registry::registerEntity, this );

    sol::table types = lua[ "bluebear" ][ "entity" ][ "types" ] = lua.create_table();
    Component::submitLuaContributions( lua, types );
    Entity::submitLuaContributions( lua, types );
    Components::ModelManager::submitLuaContributions( lua, types );
  }

  void Registry::registerComponent( const std::string& id, sol::table table ) {
    if( !componentRegistered( id ) ) {
      components[ id ] = table;
      Log::getInstance().debug( "Registry::registerComponent", "Registered component " + id );
    } else {
      Log::getInstance().warn( "Registry::registerComponent", "Component " + id + " already registered!" );
    }
  }

  void Registry::registerEntity( const std::string& id, sol::table componentlist ) {
    if( !entityRegistered( id ) ) {
      std::vector< std::string > list;

      // Verify all attached components exist and have been registered
      for( auto& pair : componentlist ) {
        if( pair.second.is< std::string >() ) {
          std::string component = pair.second.as< std::string >();
          if( !componentRegistered( component ) ) {
            Log::getInstance().error( "Registry::registerEntity", "Component " + component + " has not been registered!" );
            return;
          } else {
            list.push_back( component );
          }
        }
      }

      entities[ id ] = list;
      Log::getInstance().debug( "Registry::registerEntity", "Registered entity " + id );
    } else {
      Log::getInstance().warn( "Registry::registerEntity", "Entity " + id + " already registered!" );
    }
  }

  bool Registry::entityRegistered( const std::string& id ) {
    return entities.find( id ) != entities.end();
  }

  bool Registry::componentRegistered( const std::string& id ) {
    switch( Tools::Utility::hash( id.c_str() ) ) {
      case Tools::Utility::hash( "system.component.model_manager" ):
        return true;
      default:
        return components.find( id ) != components.end();
    }
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

}
