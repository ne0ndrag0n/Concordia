#include "engine.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "log.hpp"
#include "display.hpp"
#include <iostream>

int main() {
	BlueBear::Log::getInstance().info( "Main", "BlueBear ET2 (c) 2015-2016 ne0ndrag0n" );

	BlueBear::Display display;
	display.showDisplay();

	BlueBear::Engine engine;

	if ( !engine.setupRootEnvironment() ) {
		std::cerr << "Failed to load BlueBear!" << std::endl;
		return 1;
	}

	// Load a lot object
	if( engine.loadLot( "lots/lot01.json" ) ) {
		// Setup a loop!
		engine.objectLoop();
	}

	return 0;
}
