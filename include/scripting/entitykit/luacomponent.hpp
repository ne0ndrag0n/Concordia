#ifndef ENTITYKIT_LUA_COMPONENT
#define ENTITYKIT_LUA_COMPONENT

#include "scripting/entitykit/component.hpp"
#include <string>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>

namespace BlueBear::Scripting::EntityKit {

	class LuaComponent : public Component {
		sol::table table;

		sol::object get( const std::string& key );
		void set( const std::string& key, sol::object object );
		unsigned int size();

	public:
		LuaComponent( const std::string& componentId, const sol::table& table );

		void load( const Json::Value& data ) override;

		static void submitLuaContributions( sol::state& lua, sol::table types );
	};

}

#endif