#include "application.hpp"
#include "localemanager.hpp"
#include "log.hpp"
#include "state/householdgameplaystate.hpp"
#include <SFML/System.hpp>

namespace BlueBear {

  Application::Application() {
    Log::getInstance().info( "Application::Application", LocaleManager::getInstance().getString( "BLUEBEAR_WELCOME_MESSAGE" ) );
    sf::err().rdbuf( NULL );
  }

  void Application::close() {
    // On next loop, the application will simply close itself
    currentState = nullptr;
  }

  void Application::setupMainState() {
    currentState = std::make_unique< State::HouseholdGameplayState >( *this );
  }

  int Application::run() {
    while( currentState ) {
      currentState->update();
    }

    return 0;
  }

}
