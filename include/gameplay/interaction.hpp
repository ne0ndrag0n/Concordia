#ifndef GAMEPLAY_INTERACTION
#define GAMEPLAY_INTERACTION

#include "scripting/coreengine.hpp"
#include <sol.hpp>
#include <string>

namespace BlueBear::Gameplay {

	struct Interaction {
		std::string id;
		std::string label;
		Scripting::CoreEngine::Callback callback;

		Interaction( const std::string& id, const std::string& label, const Scripting::CoreEngine::Callback& callback );

		bool operator<( const Interaction& rhs ) const {
			return id < rhs.id;
		};

		static void submitLuaContributions( sol::table types );
	};

}

#endif