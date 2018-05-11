#include "scripting/entitykit/registry.hpp"
#include "scripting/entitykit/entity.hpp"
#include "scripting/entitykit/component.hpp"
#include "scripting/luakit/dynamicusertype.hpp"
#include "graphics/scenegraph/model.hpp"
#include "eventmanager.hpp"
#include <functional>

namespace BlueBear::Scripting::EntityKit {

  Registry::Registry() {
    eventManager.LUA_STATE_READY.listen( this, std::bind( &Registry::submitLuaContributions, this, std::placeholders::_1 ) );
  }

  Registry::~Registry() {
    eventManager.LUA_STATE_READY.stopListening( this );
  }

  void Registry::submitLuaContributions( sol::state& lua ) {
    if( lua[ "bluebear" ][ "entity" ] == sol::nil ) {
      lua[ "bluebear" ][ "entity" ] = lua.create_table();
    }

    sol::table entity = lua[ "bluebear" ][ "entity" ];

    entity.set_function( "register_component", &Registry::registerComponent, this );

    sol::table types = lua[ "bluebear" ][ "entity" ][ "types" ] = lua.create_table();
    types.new_usertype< EntityKit::Component >( "Component",
      "new", sol::no_constructor,
      sol::meta_function::index, &LuaKit::DynamicUsertype::get,
      sol::meta_function::new_index, &LuaKit::DynamicUsertype::set,
      sol::meta_function::length, &LuaKit::DynamicUsertype::size,
      sol::base_classes, sol::bases< LuaKit::DynamicUsertype >()
    );
    types.new_usertype< EntityKit::Entity >( "Entity",
      "new", sol::no_constructor,
      "get_component", &Entity::getComponent
    );

    lua[ "__test" ] = Component{};
  }

  void Registry::registerComponent( const std::string& id, sol::table table ) {

  }

  void Registry::registerEntity( const std::string& id, sol::table table ) {

  }

}
