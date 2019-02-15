#ifndef MAIN_GAME_STATE
#define MAIN_GAME_STATE

#include "state/state.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include "device/display/adapter/component/guicomponent.hpp"
#include "scripting/coreengine.hpp"
#include "scripting/luakit/eventhelper.hpp"
#include "scripting/entitykit/registry.hpp"
#include "exceptions/genexc.hpp"
#include "device/input/input.hpp"
#include "models/utilities/worldcache.hpp"
#include "models/infrastructure.hpp"
#include "gameplay/infrastructuremanager.hpp"
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
      Device::Display::Adapter::Component::WorldRenderer worldRenderer;
      Device::Display::Adapter::Component::GuiComponent guiComponent;

      Scripting::CoreEngine engine;
      Scripting::LuaKit::EventHelper luaEventHelper;
      Scripting::EntityKit::Registry entityRegistry;

      Device::Input::Input::KeyGroup keyGroup;

      Models::Utilities::WorldCache worldCache;
      Models::World world;
      Gameplay::InfrastructureManager infrastructureManager;

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
      Device::Display::Adapter::Component::WorldRenderer& getWorldRenderer();
      Models::Utilities::WorldCache& getWorldCache();

      void update() override;
    };

  }
}

#endif
