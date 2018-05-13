#include "scripting/entitykit/components/modelmanager.hpp"
#include "graphics/scenegraph/model.hpp"
#include "log.hpp"

namespace BlueBear::Scripting::EntityKit::Components {

  void ModelManager::submitLuaContributions( sol::state& lua, sol::table types ) {
    types.new_usertype< ModelManager >( "ModelManager",
      "new", sol::no_constructor,
      "init", &ModelManager::init,
      "get_potential_models", &ModelManager::getPotentialModels,
      "get_instance", &ModelManager::getInstance,
      sol::meta_function::index, &LuaKit::DynamicUsertype::get,
      sol::meta_function::new_index, &LuaKit::DynamicUsertype::set,
      sol::meta_function::length, &LuaKit::DynamicUsertype::size,
      sol::base_classes, sol::bases< LuaKit::DynamicUsertype, Component >()
    );
  }

  void ModelManager::init( sol::object incoming ) {
    if( incoming.is< sol::table >() ) {
      sol::table asTable = incoming.as< sol::table >();
      for( auto& pair : asTable ) {
        if( pair.second.is< std::string >() ) {
          potentialModels.push_back( pair.second.as< std::string >() );
        }
      }
    }
  }

  std::vector< std::string > ModelManager::getPotentialModels() {
    return potentialModels;
  }

  std::shared_ptr< Graphics::SceneGraph::Model > ModelManager::getInstance( const std::string& id ) {
    auto it = instances.find( id );
    if( it == instances.end() ) {
      throw ModelNotFoundException();
    }

    return it->second;
  }

}
