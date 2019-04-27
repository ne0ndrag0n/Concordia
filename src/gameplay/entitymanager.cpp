#include "gameplay/entitymanager.hpp"
#include "tools/utility.hpp"
#include "log.hpp"
#include <jsoncpp/json/json.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>

namespace BlueBear::Gameplay {

	Json::Value EntityManager::save() {
		// TODO
	}

	void EntityManager::load( const Json::Value& data ) {
		if( data != Json::Value::null ) {
			const Json::Value& entities = data[ "entities" ];
			if( entities.isArray() ) {
				for( const Json::Value& object : entities ) {
					const std::string& entityId = object[ "id" ].asString();
					std::shared_ptr< Scripting::EntityKit::Entity > entity = registry.createEntity( entityId, false );

					if( entity ) {
						// Unserialize components
						const Json::Value& components = object[ "components" ];
						if( components.isArray() ) {
							for( const Json::Value& componentObject : components ) {
								const std::string& componentId = componentObject[ "id" ].asString();
								std::shared_ptr< Scripting::EntityKit::Component > component = registry.createComponent( componentId, sol::nil );
								if( component ) {
									component->load( componentObject[ "data" ] );

									entity->attachComponent( component );
								} else {
									Log::getInstance().warn( "EntityManager::load", "Failed to instantiate component for entity " + entityId + ": " + componentId );
								}
							}
						}

						activeEntities.emplace_back( std::move( entity ) );
					} else {
						Log::getInstance().warn( "EntityManager::load", "Failed to instantiate entity: " + entityId );
					}
				}
			}
		}
	}

}