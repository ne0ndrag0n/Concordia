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

int main() {
	Log::getInstance().info( "Main", LocaleManager::getInstance().getString( "BLUEBEAR_WELCOME_MESSAGE" ) );
	sf::err().rdbuf( NULL );

	// ~ TESTING ~
	Log::getInstance().info( "Main", "Commencing test of some sort..." );
	float bone003cont[16] = {
		0.0143919587f, 0.0f, 1.00105202f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-1.00105202f, 0, 0.0143919587f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	glm::mat4 id( 1.0f );
	glm::mat4 bone( 1.0f );
	glm::mat4 bone002( 1.0f );
	glm::mat4 bone003 = glm::make_mat4( bone003cont );

	BlueBear::Graphics::Transform( bone003 ).printToLog();

	glm::mat4 xform =
		( bone002 * 0.0870833993f ) +
		( bone003 * 0.911155462f ) +
		( id * 0 ) +
		( id * 0 );

	glm::vec4 point = xform * glm::vec4( glm::vec3( -0.5f, -0.5f, 4.0f ), 1.0f );
	Log::getInstance().debug( "Assert", glm::to_string( point ) );
	return 0;

	EventManager eventManager;

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
