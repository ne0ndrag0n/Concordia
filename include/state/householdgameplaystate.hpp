#ifndef MAIN_GAME_STATE
#define MAIN_GAME_STATE

#include "state/state.hpp"
#include "scripting/coreengine.hpp"
#include "scripting/luakit/eventhelper.hpp"
#include "scripting/entitykit/registry.hpp"
#include "exceptions/genexc.hpp"
#include "device/input/input.hpp"
#include "models/infrastructure.hpp"
#include "models/world.hpp"
#include "serializable.hpp"
#include <memory>
#include <string>
#include <optional>

namespace BlueBear {
  class Application;

  namespace Scripting {
    class InfrastructureFactory;
  }

  namespace State {

    class HouseholdGameplayState : public State, public Serializable {
      static const unsigned int RENDER3D_ADAPTER = 0;
      static const unsigned int GUI_ADAPTER = 1;

      Scripting::CoreEngine engine;
      Scripting::LuaKit::EventHelper luaEventHelper;
      Scripting::EntityKit::Registry entityRegistry;
      Device::Input::Input::KeyGroup keyGroup;
      std::optional< Models::World > world;
      std::optional< Models::Infrastructure > infrastructure;

      void setupDisplayDevice();
      void setupInputDevice();

    public:
      EXCEPTION_TYPE( LotNotFoundException, "Lot not found!" );
      EXCEPTION_TYPE( EngineLoadFailureException, "Failed to load BlueBear!" );
      HouseholdGameplayState( Application& application, const std::string& path = "" );
      ~HouseholdGameplayState();

      Json::Value save() override;
      void load( const Json::Value& data ) override;

      Scripting::CoreEngine& getEngine();
      void update() override;
    };

  }
}

#endif
