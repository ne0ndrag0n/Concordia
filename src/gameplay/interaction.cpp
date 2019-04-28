#include "gameplay/interaction.hpp"

namespace BlueBear::Gameplay {

	Interaction::Interaction( const std::string& id, const std::string& label, const Scripting::CoreEngine::Callback& callback ) :
		id( id ), label( label ), callback( callback ) {}

	void Interaction::submitLuaContributions( sol::table types ) {
		types.new_usertype< Interaction >( "Interaction",
			sol::constructors< Interaction( const std::string&, const std::string&, const Scripting::CoreEngine::Callback& ) >(),
			"id", &Interaction::id,
			"label", &Interaction::label,
			"callback", &Interaction::callback
		);
	}

}