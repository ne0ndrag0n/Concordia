#ifndef INTERACTION_SET_COMPONENT
#define INTERACTION_SET_COMPONENT

#include "scripting/entitykit/components/modelmanager.hpp"
#include "scripting/entitykit/systemcomponent.hpp"
#include "gameplay/interaction.hpp"
#include <memory>

namespace BlueBear::Scripting::EntityKit::Components {

	class InteractionSet : public SystemComponent {
		std::map< std::string, std::vector< Gameplay::Interaction > > interactions;
		std::shared_ptr< ModelManager > modelManager;

		void refresh();

	public:
		InteractionSet();

		void associateComponent( Component& component );
		void associateInteraction( const std::string& modelId, const Gameplay::Interaction& interaction );
	};

}

#endif