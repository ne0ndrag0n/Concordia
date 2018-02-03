#ifndef MAIN_GAME_STATE
#define MAIN_GAME_STATE

#include "state/state.hpp"
#include "exceptions/genexc.hpp"
#include <memory>

namespace BlueBear {
  class Application;

  namespace Scripting {
    class Engine;
    class InfrastructureFactory;
  }

  namespace State {

    class HouseholdGameplayState : public State {
      static const unsigned int RENDER3D_ADAPTER = 0;
      static const unsigned int GUI_ADAPTER = 1;

      std::unique_ptr< Scripting::Engine > engine;
      std::unique_ptr< Scripting::InfrastructureFactory > infrastructureFactory;

      void setupEngine();
      void setupDisplayDevice();
      void setupInputDevice();

    public:
      EXCEPTION_TYPE( LotNotFoundException, "Lot not found!" );
      EXCEPTION_TYPE( EngineLoadFailureException, "Failed to load BlueBear!" );
      HouseholdGameplayState( Application& application );
      ~HouseholdGameplayState();

      Scripting::InfrastructureFactory& getInfrastructureFactory();
      void update() override;
    };

  }
}

#endif
