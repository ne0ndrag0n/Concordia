#include "scripting/entitykit/luacomponent.hpp"
#include "scripting/luakit/utility.hpp"
#include "tools/utility.hpp"
#include "log.hpp"

namespace BlueBear::Scripting::EntityKit {

	LuaComponent::LuaComponent( const std::string& componentId, const sol::table& table ) : Component::Component( componentId ) {
		this->table = table;
	}

	void LuaComponent::load( const Json::Value& data ) {
		if( data != Json::Value::null ) {
			sol::object object = get( "load" );

			if( object.is< sol::function >() ) {
				LuaKit::Utility::cast< sol::function >( object )( *this, Tools::Utility::jsonToString( data ) );
			}
		}
	}

	void LuaComponent::init( sol::object object ) {
		sol::object potentialFunction = table[ "init" ];
		if( potentialFunction.is< sol::function >() ) {
			table[ "init" ]( *this, object );
		}
	}

	void LuaComponent::submitLuaContributions( sol::state& lua, sol::table types ) {
		types.new_usertype< EntityKit::LuaComponent >( "LuaComponent",
			"new", sol::no_constructor,
			sol::meta_function::index, &LuaComponent::get,
			sol::meta_function::new_index, &LuaComponent::set,
			sol::meta_function::length, &LuaComponent::size,
			sol::base_classes, sol::bases< Component >()
		);
	}

	sol::object LuaComponent::get( const std::string& key ) {
		return table[ key ];
	}

	void LuaComponent::set( const std::string& key, sol::object object ) {
		table[ key ] = object;
	}

	unsigned int LuaComponent::size() {
		return table.size();
	}

}