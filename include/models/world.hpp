#ifndef WORLD_MODEL
#define WORLD_MODEL

#include "serializable.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <set>

namespace BlueBear::Device::Display::Adapter::Component{ class WorldRenderer; }
namespace BlueBear::Scripting::EntityKit{ class Entity; }
namespace BlueBear::Graphics::SceneGraph{ class Model; }
namespace BlueBear::Models {

  class World : public Serializable {
  public:
    struct Object {
      std::shared_ptr< Scripting::EntityKit::Entity > associatedEntity;
      std::shared_ptr< Graphics::SceneGraph::Model > model;
    };

  private:
    Device::Display::Adapter::Component::WorldRenderer& worldRenderer;
    std::vector< Object > objects;

  public:
    World( Device::Display::Adapter::Component::WorldRenderer& worldRenderer );
    ~World();

    void submitLuaContributions( sol::state& lua );
    void removeObject( std::shared_ptr< Scripting::EntityKit::Entity > comparison );
    void removeObject( std::shared_ptr< Graphics::SceneGraph::Model > byModel );

    Json::Value save() override;
    void load( const Json::Value& data ) override;
  };

}

#endif
