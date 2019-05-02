#include "scripting/entitykit/registry.hpp"
#include "scripting/entitykit/entity.hpp"
#include "scripting/entitykit/luacomponent.hpp"
#include "scripting/entitykit/components/modelmanager.hpp"
#include "scripting/entitykit/components/interactionset.hpp"
#include "scripting/luakit/utility.hpp"
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

  void Registry::submitLuaContributions( sol::state& lua ) {
    engineState = &lua;

    if( lua[ "bluebear" ][ "entity" ] == sol::nil ) {
      lua[ "bluebear" ][ "entity" ] = lua.create_table();
    }

    sol::table entity = lua[ "bluebear" ][ "entity" ];

    entity.set_function( "register_component", &Registry::registerComponent, this );
    entity.set_function( "register_entity", &Registry::registerEntity, this );

    sol::table types = lua[ "bluebear" ][ "entity" ][ "types" ] = lua.create_table();
    Component::submitLuaContributions( lua, types );
    LuaComponent::submitLuaContributions( lua, types );
    Entity::submitLuaContributions( lua, types );
    Components::ModelManager::submitLuaContributions( lua, types );
    Components::InteractionSet::submitLuaContributions( lua, types );
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
      case Tools::Utility::hash( "system.component.interaction_set" ):
        return true;
      default:
        return components.find( id ) != components.end();
    }
  }

  std::shared_ptr< Entity > Registry::createEntity( const std::string& registeredId, bool defaults ) {
    auto it = entities.find( registeredId );
    if( it == entities.end() ) {
      Log::getInstance().debug( "Registry::createEntity", "Entity type not registered: " + registeredId );
      return nullptr;
    }

    if( defaults ) {
      std::shared_ptr< Entity > entity;

      for( const auto& stringId : it->second ) {
        std::shared_ptr< Component > component = createComponent( stringId, sol::nil );
        if ( !component ) {
          Log::getInstance().warn( "Registry::createEntity", "Attempted to add component " + stringId + " but component is not registered." );
        } else {
          entity->attachComponent( component );
        }
      }

      return entity;
    } else {
      return std::make_shared< Entity >( registeredId, std::vector< std::shared_ptr< Component > >{} );
    }
  }

  std::shared_ptr< Component > Registry::createComponent( const std::string& registeredId, sol::object initArgs ) {
    switch( Tools::Utility::hash( registeredId.c_str() ) ) {
      case Tools::Utility::hash( "system.component.model_manager" ): {
        return std::make_shared< Components::ModelManager >();
      }
      case Tools::Utility::hash( "system.component.interaction_set" ): {
        return std::make_shared< Components::InteractionSet >();
      }
      default: {
        auto it = components.find( registeredId );
        if( it == components.end() ) {
          Log::getInstance().debug( "Registry::createComponent", "Component not registered: " + registeredId );
          return nullptr;
        }

        std::shared_ptr< LuaComponent > luaComponent = std::make_shared< LuaComponent >( registeredId, LuaKit::Utility::copyTable( *engineState, it->second, true ) );
        luaComponent->init( initArgs );

        return luaComponent;
      }
    }
  }

}
