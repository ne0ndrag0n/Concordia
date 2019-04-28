#ifndef GAMEPLAY_INTERACTION
#define GAMEPLAY_INTERACTION

#include "scripting/coreengine.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <string>

namespace BlueBear::Gameplay {

	struct Interaction {
		std::string id;
		std::string label;
		Scripting::CoreEngine::Callback callback;

		Interaction( const std::string& id, const std::string& label, const Scripting::CoreEngine::Callback& callback );

		static void submitLuaContributions( sol::table types );
	};

}

#endif