#include "scripting/engine.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "log.hpp"
#include "graphics/display.hpp"
#include "localemanager.hpp"
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <memory>

using namespace BlueBear;

int main() {
	Log::getInstance().info( "Main", LocaleManager::getInstance().getString( "BLUEBEAR_WELCOME_MESSAGE" ) );
	sf::err().rdbuf( NULL );

	Graphics::Display display;
	Scripting::Engine engine;

	display.openDisplay();

	if ( !engine.setupRootEnvironment() ) {
		Log::getInstance().error( "main", "Failed to load BlueBear!" );
		return 1;
	}

	// Load a lot object
	if( !engine.loadLot( "lots/lot01.json" ) ) {
		Log::getInstance().error( "main", "Failed to load demo lot!" );
	}

	// Fully de-threaded..."functional decomposition" turned out to be shite
	// Keep the application responsive by splitting out heavy-duty tasks into threads, "Destiny" style
	// TODO: Dump amanieu/asyncplusplus for Intel TBB or C++17

	// TODO: Display state transition 
	bool active = true;
	while( active ) {
		// update the game state first
		engine.objectLoop();

		// then render the game
		active = display.update();
	}

	return 0;
}
