#include "bluebear.hpp"
#include <iostream>
#include <cstdio>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

int main() {
	BlueBear::Engine engine;
	
	if ( !engine.setupRootEnvironment() ) {
		std::cerr << "Failed to load BlueBear!" << std::endl;
		return 1;
	}

	// Load a lot object
	if( engine.loadLot( "lots/lot01.bbl" ) ) {
		// Setup a loop!
		engine.objectLoop();
	}
	
	return 0;
}