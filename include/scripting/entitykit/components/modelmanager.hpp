#ifndef COMPONENT_MODELMANAGER
#define COMPONENT_MODELMANAGER

#include "exceptions/genexc.hpp"
#include "scripting/entitykit/systemcomponent.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace BlueBear::Device::Display::Adapter::Component{ class WorldRenderer; }
namespace BlueBear::Graphics::SceneGraph{ class Model; }
namespace BlueBear::Scripting::EntityKit::Components {

  class ModelManager : public SystemComponent {
    std::vector< std::string > potentialModels;

  public:
    EXCEPTION_TYPE( ModelNotFoundException, "Local ID not found in ModelManager!" );

    ModelManager();

    static BlueBear::Device::Display::Adapter::Component::WorldRenderer* worldRenderer;
    static void submitLuaContributions( sol::state& lua, sol::table types );

    void init( sol::object incoming ) override;
    void load( const Json::Value& data ) override;

    std::vector< std::string > getPotentialModels();
  };

}

#endif
