#include "scripting/engine.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "log.hpp"
#include "graphics/display.hpp"
#include "threading/commandbus.hpp"
#include "localemanager.hpp"
#include <thread>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <memory>
#include <xercesc/util/PlatformUtils.hpp>

using namespace BlueBear;

int main() {
	Log::getInstance().info( "Main", LocaleManager::getInstance().getString( "BLUEBEAR_WELCOME_MESSAGE" ) );
	sf::err().rdbuf( NULL );

	// Initialize xerces XML platform utils
	try {
		xercesc::XMLPlatformUtils::Initialize();
	} catch( const xercesc::XMLException& e ) {
		Log::getInstance().error( "main", "Failed to initialize Apache Xerces: " + std::string( (const char *) e.getMessage() ) );
		return 2;
	}

	// main() thread is the UI thread, or should be
	Threading::CommandBus commandBus;
	Graphics::Display display( commandBus );
	display.openDisplay();

	Scripting::Engine engine( commandBus );
	if ( !engine.setupRootEnvironment() ) {
		Log::getInstance().error( "main", "Failed to load BlueBear!" );
		return 1;
	}
	// Load a lot object
	std::thread engineThread;
	if( engine.loadLot( "lots/lot01.json" ) ) {
		engineThread = std::thread( &Scripting::Engine::objectLoop, &engine );
	}

	display.start();

	engineThread.join();

	xercesc::XMLPlatformUtils::Terminate();
	return 0;
}
