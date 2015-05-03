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
	}
	
	return 0;
}
