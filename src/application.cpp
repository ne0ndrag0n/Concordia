#include "application.hpp"
#include "localemanager.hpp"
#include "log.hpp"
#include "scripting/lot.hpp"
#include <SFML/System.hpp>

namespace BlueBear {

  /**
   * The contents of this will change after the refactor
   */
  Application::Application() {
    Log::getInstance().info( "Main", LocaleManager::getInstance().getString( "BLUEBEAR_WELCOME_MESSAGE" ) );
    sf::err().rdbuf( NULL );

    engine = getEngine();
    if( !engine->submitLuaContributions() ) {
      Log::getInstance().error( "Application::Application", "Failed to load BlueBear!" );
      throw EngineLoadFailureException();
    }

    if( !engine->loadLot( "lots/lot01.json" ) ) {
      Log::getInstance().error( "Application::Application", "Failed to load demo lot!" );
      throw LotNotFoundException();
    }

    display = getDisplay();
    display->openDisplay();
  }

  std::unique_ptr< Scripting::Engine > Application::getEngine() {
    return std::make_unique< Scripting::Engine >();
  }

  std::unique_ptr< Graphics::Display > Application::getDisplay() {
    return std::make_unique< Graphics::Display >( engine.get() );
  }

  void Application::setupMainState() {
    display->changeToMainGameState( engine->currentLot->currentRotation, *( engine->currentLot )->floorMap, *( engine->currentLot )->wallMap );
  }

  int Application::run() {
    do {
      engine->objectLoop();
    } while( display->update() );

    // unix status code
    return 0;
  }

}
