#ifndef COMPONENT_CAST
#define COMPONENT_CAST

#include "scripting/entitykit/component.hpp"
#include "scripting/entitykit/components/modelmanager.hpp"
#include "scripting/entitykit/components/interactionset.hpp"
#include <memory>
#include <variant>

namespace BlueBear::Scripting::EntityKit::Components {

  using ComponentReturn = std::variant<
    std::shared_ptr< Component >,
    std::shared_ptr< ModelManager >,
    std::shared_ptr< InteractionSet >
  >;

  // FUCKING
  // BARF
  static ComponentReturn cast( std::shared_ptr< Component > component ) {
    if( auto candidate = std::dynamic_pointer_cast< ModelManager >( component ) ) {
      return candidate;
    } else if( auto candidate = std::dynamic_pointer_cast< InteractionSet >( component ) ) {
      return candidate;
    } else {
      return component;
    }
  };

}

#endif
