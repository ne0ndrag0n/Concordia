#include "scripting/engine.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "log.hpp"
#include "graphics/display.hpp"
#include "localemanager.hpp"
#include <iostream>
#include <thread>
#include <SFML/Window.hpp>

using namespace BlueBear;

int main() {
	Log::getInstance().info( "Main", LocaleManager::getInstance().getString( "BLUEBEAR_WELCOME_MESSAGE" ) );

	// main() thread is the UI thread, or should be
	Graphics::Display display;
	display.openDisplay();

	Scripting::Engine engine;
	if ( !engine.setupRootEnvironment() ) {
		std::cerr << "Failed to load BlueBear!" << std::endl;
		return 1;
	}
	// Load a lot object
	std::thread engineThread;
	if( engine.loadLot( "lots/lot01.json" ) ) {
		engineThread = std::thread( &Scripting::Engine::objectLoop, &engine );
	}

	while( display.isOpen() ) {
		display.render();
	}

	engineThread.join();

	return 0;
}
