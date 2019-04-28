#ifndef INTERACTION_SET_COMPONENT
#define INTERACTION_SET_COMPONENT

#include "scripting/entitykit/components/modelmanager.hpp"
#include "scripting/entitykit/systemcomponent.hpp"
#include "graphics/scenegraph/model.hpp"
#include "gameplay/interaction.hpp"
#include "device/input/input.hpp"
#include <memory>
#include <set>

namespace BlueBear::Graphics::SceneGraph { class Model; }
namespace BlueBear::Scripting::EntityKit::Components {

	class InteractionSet : public SystemComponent {
		struct EventDescriptor {
			std::shared_ptr< Graphics::SceneGraph::Model > model;
			std::vector< std::pair< std::string, unsigned int > > registeredEvents;

			bool operator<( const EventDescriptor& rhs ) const {
				return model.get() < rhs.model.get();
			};
		};

		std::map< std::string, std::vector< Gameplay::Interaction > > interactions;
		std::set< EventDescriptor > previousItems;
		std::shared_ptr< ModelManager > modelManager;

		void refresh();

		void modelMouseIn( Device::Input::Metadata event, std::shared_ptr< Graphics::SceneGraph::Model > model );
        void modelMouseOut( Device::Input::Metadata event, std::shared_ptr< Graphics::SceneGraph::Model > model );

	public:
		InteractionSet();

		void associateComponent( Component& component );
		void associateInteraction( const std::string& modelId, const Gameplay::Interaction& interaction );
	};

}

#endif