#include "scripting/engine.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "log.hpp"
#include "graphics/display.hpp"
#include "graphics/transform.hpp"
#include "localemanager.hpp"
#include "eventmanager.hpp"
#include "scripting/lot.hpp"
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <memory>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

using namespace BlueBear;

void test() {
	std::map< std::weak_ptr< int >, std::string, std::owner_less< std::weak_ptr< int > > > test;

	std::shared_ptr< int > five = std::make_shared< int >( 5 );
	std::shared_ptr< int > differentFive = std::make_shared< int >( 5 );

	test[ five ] = "five";

	Log::getInstance().debug( "", test[ five ] );
	Log::getInstance().debug( "", test.find( differentFive ) == test.end() ? "true" : "false" );
	Log::getInstance().debug( "", std::to_string( test.begin()->first.expired() ) );

	five = nullptr;

	Log::getInstance().debug( "", std::to_string( test.begin()->first.expired() ) );
}

int main() {
	Log::getInstance().info( "Main", LocaleManager::getInstance().getString( "BLUEBEAR_WELCOME_MESSAGE" ) );
	sf::err().rdbuf( NULL );

	std::shared_ptr< EventManager > eventManager = std::make_shared< EventManager >();

	Scripting::Engine engine( eventManager );
	if ( !engine.submitLuaContributions() ) {
		Log::getInstance().error( "main", "Failed to load BlueBear!" );
		return 1;
	}
	// Load a lot object
	if( !engine.loadLot( "lots/lot01.json" ) ) {
		Log::getInstance().error( "main", "Failed to load demo lot!" );
	}

	Graphics::Display display( engine.L, eventManager );
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
