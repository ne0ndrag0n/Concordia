#ifndef MAIN_GAME_STATE
#define MAIN_GAME_STATE

#include "state/state.hpp"
#include "exceptions/genexc.hpp"
#include <memory>

namespace BlueBear {
  class Application;

  namespace Graphics {
    class Display;
  }

  namespace Scripting {
    class Engine;
    class InfrastructureFactory;
  }

  namespace State {

    class HouseholdGameplayState : public State {
      std::unique_ptr< Scripting::Engine > engine;
      std::unique_ptr< Graphics::Display > display;
      std::unique_ptr< Scripting::InfrastructureFactory > infrastructureFactory;

    public:
      EXCEPTION_TYPE( LotNotFoundException, "Lot not found!" );
      EXCEPTION_TYPE( EngineLoadFailureException, "Failed to load BlueBear!" );
      HouseholdGameplayState( Application& application );
      ~HouseholdGameplayState();

      Scripting::InfrastructureFactory& getInfrastructureFactory();
      void update() override;

      // TODO: Replace
      void newUpdate();
    };

  }
}

#endif
