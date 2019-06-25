#ifndef COMPONENT_MODELMANAGER
#define COMPONENT_MODELMANAGER

#include "exceptions/genexc.hpp"
#include "scripting/entitykit/systemcomponent.hpp"
#include <sol.hpp>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

namespace BlueBear::Graphics::SceneGraph{ class Model; }
namespace BlueBear::Scripting::EntityKit::Components {

  class ModelManager : public SystemComponent {
    std::vector< std::string > potentialModels;
    std::vector< std::pair< std::string, std::shared_ptr< Graphics::SceneGraph::Model > > > models;

  public:
    EXCEPTION_TYPE( ModelNotFoundException, "Local ID not found in ModelManager!" );

    ModelManager();

    static void submitLuaContributions( sol::state& lua, sol::table types );

    void init( sol::object incoming ) override;
    void load( const Json::Value& data ) override;
    void drop() override;

    std::shared_ptr< Graphics::SceneGraph::Model > placeObject( const std::string& modelId, sol::table classes );
    void removeObject( Graphics::SceneGraph::Model& model );

    std::vector< std::string > getPotentialModels();
    std::vector< std::pair< std::string, std::shared_ptr< Graphics::SceneGraph::Model > > > getModels() const;
  };

}

#endif
