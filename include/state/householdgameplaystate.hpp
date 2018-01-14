#ifndef MAIN_GAME_STATE
#define MAIN_GAME_STATE

#include "state/state.hpp"
#include "exceptions/genexc.hpp"
#include "graphics/display.hpp"
#include "scripting/engine.hpp"
#include <memory>

namespace BlueBear {
  class Application;

  namespace State {

    class HouseholdGameplayState : public State {
      std::unique_ptr< Scripting::Engine > engine;
      std::unique_ptr< Graphics::Display > display;
      // TODO: Reserved for infrastructure source

    public:
      EXCEPTION_TYPE( LotNotFoundException, "Lot not found!" );
      EXCEPTION_TYPE( EngineLoadFailureException, "Failed to load BlueBear!" );
      HouseholdGameplayState( Application& application );

      void update() override;
    };

  }
}

#endif
