#include "scripting/entitykit/components/modelmanager.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include "graphics/scenegraph/model.hpp"
#include "tools/utility.hpp"
#include "log.hpp"

namespace BlueBear::Scripting::EntityKit::Components {

  BlueBear::Device::Display::Adapter::Component::WorldRenderer* ModelManager::worldRenderer = nullptr;

  void ModelManager::submitLuaContributions( sol::state& lua, sol::table types ) {
    types.new_usertype< ModelManager >( "ModelManager",
      "new", sol::no_constructor,
      "init", &ModelManager::init,
      "get_potential_models", &ModelManager::getPotentialModels,
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

  void ModelManager::load( const Json::Value& data ) {
    std::vector< std::pair< std::string, std::string > > paths;

    for( auto it = data.begin(); it != data.end(); ++it ) {
      auto pair = Tools::Utility::jsonIteratorToPair( it );

      potentialModels.emplace_back( pair.first );
      paths.emplace_back( pair.first, pair.second.get().asString() );
    }

    if( paths.size() ) {
      worldRenderer->loadPathsParallel( paths );
    }
  }

  std::vector< std::string > ModelManager::getPotentialModels() {
    return potentialModels;
  }

}
