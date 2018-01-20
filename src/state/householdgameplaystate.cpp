#include "state/householdgameplaystate.hpp"
#include "log.hpp"
#include "application.hpp"
#include "scripting/lot.hpp"
#include "scripting/infrastructurefactory.hpp"
#include "scripting/engine.hpp"
#include "graphics/display.hpp"
#include "scripting/luastate.hpp"

namespace BlueBear {
  namespace State {

    HouseholdGameplayState::HouseholdGameplayState( Application& application ) : State::State( application ) {
      infrastructureFactory = std::make_unique< Scripting::InfrastructureFactory >();

      engine = std::make_unique< Scripting::Engine >( *this );
      if( !engine->submitLuaContributions() ) {
        Log::getInstance().error( "HouseholdGameplayState::HouseholdGameplayState", "Failed to load BlueBear!" );
        throw EngineLoadFailureException();
      }

      if( !engine->loadLot( "lots/lot01.json" ) ) {
        Log::getInstance().error( "HouseholdGameplayState::HouseholdGameplayState", "Failed to load demo lot!" );
        throw LotNotFoundException();
      }

      L = engine->L;
      display = std::make_unique< Graphics::Display >( *this, engine.get() );
      display->openDisplay();
      display->changeToMainGameState( engine->currentLot->currentRotation, *( engine->currentLot )->floorMap, *( engine->currentLot )->wallMap );
    }

    HouseholdGameplayState::~HouseholdGameplayState() {}

    Scripting::InfrastructureFactory& HouseholdGameplayState::getInfrastructureFactory() {
      return *infrastructureFactory;
    }

    void HouseholdGameplayState::update() {
      engine->update();
      if( !display->update() ) {
        application.close();
      }
    }

  }
}
