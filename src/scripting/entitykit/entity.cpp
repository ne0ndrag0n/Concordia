#include "scripting/entitykit/entity.hpp"
#include "tools/utility.hpp"
#include "log.hpp"

namespace BlueBear::Scripting::EntityKit {

  Entity::Entity( std::map< std::string, std::shared_ptr< Component > > components ) : components( components ) {
    associate( this );
  }

  Entity::Entity( const Entity& entity ) {
    components = entity.components;

    associate( this );
  }

  Entity::~Entity() {
    // These objects should become useless to use by Lua, but not crash the game
    associate( nullptr );
  }

  Json::Value Entity::save() {
    return {};
  }

  void Entity::load( const Json::Value& data ) {
    if( data != Json::Value::null ) {
      const Json::Value& componentsJson = data[ "components" ];
      if( componentsJson != Json::Value::null && componentsJson.isObject() ) {
        for( auto it = componentsJson.begin(); it != componentsJson.end(); ++it ) {
          auto pair = Tools::Utility::jsonIteratorToPair( it );
          auto instance = components.find( pair.first );
          if( instance != components.end() ) {
            instance->second->load( pair.second );
          } else {
            Log::getInstance().error( "Entity::load", "Did not find component " + pair.first + " attached to this entity" );
          }
        }
      }
    }
  }

  void Entity::submitLuaContributions( sol::state& lua, sol::table types ) {
    types.new_usertype< EntityKit::Entity >( "Entity",
      "new", sol::no_constructor,
      "get_component", &Entity::getComponent
    );
  }

  void Entity::associate( Entity* pointer ) {
    for( auto& pair : components ) {
      pair.second->attach( pointer );
    }
  }

  Components::ComponentReturn Entity::getComponent( const std::string& componentId ) {
    auto it = components.find( componentId );
    if( it == components.end() ) {
      throw NotFoundException();
    }

    return Components::cast( it->second );
  }

  void Entity::close() {
    for( auto& pair : components ) {
      auto& component = *( pair.second );
      sol::object potentialFunction = component[ "close" ];
      if( potentialFunction.is< sol::function >() ) {
        potentialFunction.as< sol::function >()( component );
      }
    }

    associate( nullptr );
  }

}
