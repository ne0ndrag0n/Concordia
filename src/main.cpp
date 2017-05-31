#include "scripting/engine.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "log.hpp"
#include "graphics/display.hpp"
#include "graphics/transform.hpp"
#include "localemanager.hpp"
#include "scripting/lot.hpp"
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <memory>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <regex>

using namespace BlueBear;

void test() {
	std::string matches( "asdsa:fsdgsd" );
	std::string nomatches( "asdsafsdgsd" );

	Log::getInstance().debug( "matches", matches.find( ":" ) == std::string::npos ? "true" : "false" );
	Log::getInstance().debug( "nomatches", nomatches.find( ":" ) == std::string::npos ? "true" : "false" );
}

int main() {
	Log::getInstance().info( "Main", LocaleManager::getInstance().getString( "BLUEBEAR_WELCOME_MESSAGE" ) );
	sf::err().rdbuf( NULL );

	Scripting::Engine engine;
	if ( !engine.submitLuaContributions() ) {
		Log::getInstance().error( "main", "Failed to load BlueBear!" );
		return 1;
	}
	// Load a lot object
	if( !engine.loadLot( "lots/lot01.json" ) ) {
		Log::getInstance().error( "main", "Failed to load demo lot!" );
	}

	Graphics::Display display( engine.L );
	display.openDisplay();

	// send engine lot data to display
	display.changeToMainGameState( engine.currentLot->currentRotation, *engine.currentLot->floorMap, *engine.currentLot->wallMap );

	// Fully de-threaded..."functional decomposition" turned out to be shite
	// Keep the application responsive by splitting out heavy-duty tasks into threads, "Destiny" style
	bool active = true;
	while( active ) {
		// update the game state first
		engine.objectLoop();

		// then render the game
		active = display.update();
	}

	return 0;
}
