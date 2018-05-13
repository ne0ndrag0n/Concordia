#ifndef COMPONENT_MODELMANAGER
#define COMPONENT_MODELMANAGER

#include "exceptions/genexc.hpp"
#include "scripting/entitykit/component.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace BlueBear::Graphics::SceneGraph{ class Model; }
namespace BlueBear::Scripting::EntityKit::Components {

  class ModelManager : public Component {
    std::vector< std::string > potentialModels;
    std::unordered_map< std::string, std::shared_ptr< Graphics::SceneGraph::Model > > instances;

  public:
    EXCEPTION_TYPE( ModelNotFoundException, "Local ID not found in ModelManager!" );

    static void submitLuaContributions( sol::state& lua, sol::table types );

    void init( sol::object incoming ) override;
    std::vector< std::string > getPotentialModels();
    std::shared_ptr< Graphics::SceneGraph::Model > getInstance( const std::string& id );
  };

}

#endif
