#include "scripting/entitykit/component.hpp"

namespace BlueBear::Scripting::EntityKit {

  Component::Component( const std::map< std::string, sol::object >& types ) : LuaKit::DynamicUsertype( types ) {}

  void Component::attach( Entity* entity ) {
    this->entity = entity;
  }

}
