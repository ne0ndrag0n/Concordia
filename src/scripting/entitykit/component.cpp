#include "scripting/entitykit/component.hpp"
#include "scripting/entitykit/entity.hpp"
#include "log.hpp"

namespace BlueBear::Scripting::EntityKit {

  Component::Component( const std::map< std::string, sol::object >& types ) : LuaKit::DynamicUsertype( types ) {}

  void Component::submitLuaContributions( sol::state& lua, sol::table types ) {
    types.new_usertype< EntityKit::Component >( "Component",
      "new", sol::no_constructor,
      "get_entity", &Component::getEntity,
      sol::meta_function::index, &LuaKit::DynamicUsertype::get,
      sol::meta_function::new_index, &LuaKit::DynamicUsertype::set,
      sol::meta_function::length, &LuaKit::DynamicUsertype::size,
      sol::base_classes, sol::bases< LuaKit::DynamicUsertype >()
    );
  }

  Entity& Component::getEntity() {
    if( !entity ) {
      Log::getInstance().error( "Component::getEntity", "Parent entity has been destroyed!" );
      throw EntityDestroyedException();
    }

    return *entity;
  }

  void Component::attach( Entity* entity ) {
    this->entity = entity;
  }

  void Component::init( sol::object object ) {
    sol::object init = get( "init" );
    if( init.is< sol::function >() ) {
      init.as< sol::function >()( *this, object );
    }
  }

}
