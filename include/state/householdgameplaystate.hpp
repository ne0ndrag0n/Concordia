#ifndef MAIN_GAME_STATE
#define MAIN_GAME_STATE

#include "state/state.hpp"
#include "scripting/coreengine.hpp"
#include "scripting/luakit/eventhelper.hpp"
#include "exceptions/genexc.hpp"
#include "device/input/input.hpp"
#include <memory>

namespace BlueBear {
  class Application;

  namespace Scripting {
    class InfrastructureFactory;
  }

  namespace State {

    class HouseholdGameplayState : public State {
      static const unsigned int RENDER3D_ADAPTER = 0;
      static const unsigned int GUI_ADAPTER = 1;

      Scripting::CoreEngine engine;
      Scripting::LuaKit::EventHelper luaEventHelper;
      Device::Input::Input::KeyGroup keyGroup;

      void setupDisplayDevice();
      void setupInputDevice();

    public:
      EXCEPTION_TYPE( LotNotFoundException, "Lot not found!" );
      EXCEPTION_TYPE( EngineLoadFailureException, "Failed to load BlueBear!" );
      HouseholdGameplayState( Application& application );
      ~HouseholdGameplayState();

      Scripting::CoreEngine& getEngine();
      void update() override;
    };

  }
}

#endif
