#include "scripting/entitykit/component.hpp"
#include "scripting/entitykit/entity.hpp"
#include "log.hpp"

namespace BlueBear::Scripting::EntityKit {

  Component::Component( const std::string& componentId ) : componentId( componentId ) {}

  Json::Value Component::save() {
    return Json::Value::null;
  }

  void Component::load( const Json::Value& data ) {}

  void Component::submitLuaContributions( sol::state& lua, sol::table types ) {
    types.new_usertype< EntityKit::Component >( "Component",
      "new", sol::no_constructor,
      "get_entity", &Component::getEntity,
      "get_component_id", &Component::getId
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

  const std::string& Component::getId() const {
    return componentId;
  }

  void Component::init( sol::object object ) {
    // Abstract
  }

  void Component::drop() {
    // Abstract
  }

}
