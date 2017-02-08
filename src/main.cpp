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
	// ~ TESTING ~
	Log::getInstance().info( "Main", "Commencing test of some sort..." );

	glm::mat4 id( 1.0f ); // ID 0
	glm::mat4 bone( 1.0f ); // ID 1
	glm::mat4 bone002( 1.0f ); // ID 2
	glm::mat4 bone003( 1.0f ); // ID 3

	BlueBear::Graphics::Transform t;
	t.setPosition( glm::vec3( 0.0f, 0.0f, 3.0f ) );
	t.setRotation( glm::angleAxis( (float)glm::radians( -89.113 ), glm::vec3( 0.0f, 1.0f, 0.0f ) ) );

	BlueBear::Graphics::Transform ibp;
	ibp.setPosition( glm::vec3( 0.0f, 0.0f, -3.0f ) );

	// Keyframe is set to rotate bone003 -89.113 degrees along Y
	bone003 *= t.getUpdatedMatrix(); // move into bone space
	bone003 *= ibp.getUpdatedMatrix(); // move out of bone space

	Log::getInstance().debug( "Assert", glm::to_string( bone003 ) );

	glm::mat4 xform =
		( bone002 * 0.087f ) +
		( bone003 * 0.911f ) +
		( id * 0 ) +
		( id * 0 );

	// Vertex position is -0.5, -0.5, 4.0
	glm::vec4 point = xform * glm::vec4( glm::vec3( -0.5f, -0.5f, 4.0f ), 1.0f );
	// According to blender, should be -0.95638, -0.5, 2.63086
	// Instead, result is -3.694115, -0.499000, -0.051035
	Log::getInstance().debug( "Assert", glm::to_string( point ) );
}

int main() {
	Log::getInstance().info( "Main", LocaleManager::getInstance().getString( "BLUEBEAR_WELCOME_MESSAGE" ) );
	sf::err().rdbuf( NULL );

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
