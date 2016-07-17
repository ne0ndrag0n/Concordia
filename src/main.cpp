#include "scripting/engine.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "log.hpp"
#include "graphics/display.hpp"
#include "localemanager.hpp"
#include <iostream>
#include <thread>

using namespace BlueBear;

int main() {
	Log::getInstance().info( "Main", LocaleManager::getInstance().getString( "BLUEBEAR_WELCOME_MESSAGE" ) );

	std::thread windowThread( []() {
		Graphics::Display display;
		display.showDisplay();
	} );

	Scripting::Engine engine;

	if ( !engine.setupRootEnvironment() ) {
		std::cerr << "Failed to load BlueBear!" << std::endl;
		return 1;
	}

	// Load a lot object
	if( engine.loadLot( "lots/lot01.json" ) ) {
		// Setup a loop!
		engine.objectLoop();
	}

	// Thread should take it from here
	Log::getInstance().warn( "Main", "Program execution has completed, please close the SFML window." );
	windowThread.join();

	return 0;
}
