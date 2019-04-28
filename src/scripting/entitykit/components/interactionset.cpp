#include "scripting/entitykit/components/interactionset.hpp"
#include "scripting/entitykit/entity.hpp"
#include "scripting/entitykit/components/componentcast.hpp"
#include "containers/visitor.hpp"
#include "log.hpp"

namespace BlueBear::Scripting::EntityKit::Components {

	InteractionSet::InteractionSet() : SystemComponent::SystemComponent( "system.component.interaction_set" ) {}

	void InteractionSet::refresh() {
		if( modelManager ) {

		}
	}

	void InteractionSet::associateComponent( Component& component ) {
		if( component.getId() == "system.component.model_manager" ) {
			modelManager = std::dynamic_pointer_cast< ModelManager >( component.shared_from_this() );
		} else {
			Log::getInstance().warn(
				"InteractionSet::associateComponent",
				"Can only associate system.component.model_manager to system.component.interaction_set"
			);
		}

		refresh();
	}

	void InteractionSet::associateInteraction( const std::string& modelId, const Gameplay::Interaction& interaction ) {
		interactions[ modelId ].emplace_back( interaction );

		refresh();
	}

}