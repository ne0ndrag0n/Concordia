#include "engine.hpp"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <iostream>

int main() {
	std::cout << "BlueBear v0.0.1 (c) 2015-2016 oaktree" << std::endl;

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
