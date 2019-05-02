#include "scripting/entitykit/components/interactionset.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include "graphics/scenegraph/uniforms/highlight_uniform.hpp"
#include "scripting/entitykit/entity.hpp"
#include "scripting/entitykit/components/componentcast.hpp"
#include "state/householdgameplaystate.hpp"
#include "containers/visitor.hpp"
#include "log.hpp"
#include <functional>

namespace BlueBear::Scripting::EntityKit::Components {

	InteractionSet::InteractionSet() : SystemComponent::SystemComponent( "system.component.interaction_set" ) {
		relevantState->getWorldRenderer().MODEL_REMOVED.listen( this, std::bind( &InteractionSet::modelRemoved, this, std::placeholders::_1 ) );
	}

	InteractionSet::~InteractionSet() {
		relevantState->getWorldRenderer().MODEL_REMOVED.stopListening( this );
	}

	void InteractionSet::modelMouseIn( Device::Input::Metadata event, std::shared_ptr< Graphics::SceneGraph::Model > model ) {
		Graphics::SceneGraph::Uniforms::HighlightUniform* highlighter = ( Graphics::SceneGraph::Uniforms::HighlightUniform* ) model->getUniform( "highlight" );
		highlighter->fadeTo( { 0.2f, 0.2f, 0.2f, 0.0f } );
	}

	void InteractionSet::modelMouseOut( Device::Input::Metadata event, std::shared_ptr< Graphics::SceneGraph::Model > model ) {
		Graphics::SceneGraph::Uniforms::HighlightUniform* highlighter = ( Graphics::SceneGraph::Uniforms::HighlightUniform* ) model->getUniform( "highlight" );
		highlighter->fadeTo( { 0.0f, 0.0f, 0.0f, 0.0f } );
	}

	void InteractionSet::modelRemoved( std::shared_ptr< Graphics::SceneGraph::Model > model ) {
		// Perform a simple drop of all interactions for this model
		interactions.erase( model );
	}

	void InteractionSet::updateUniformsAndEvents( std::shared_ptr< Graphics::SceneGraph::Model > instance ) {
		EventDescriptor& descriptor = interactions[ instance ];

		if( !descriptor.interactions.empty() ) {
			// Ensure events and uniforms are set up
			descriptor.registeredEvents.emplace_back(
				"mouse-in",
				relevantState->getWorldRenderer().registerEvent(
					instance, "mouse-in", std::bind( &InteractionSet::modelMouseIn, this, std::placeholders::_1, std::placeholders::_2 )
				)
			);

			descriptor.registeredEvents.emplace_back(
				"mouse-out",
				relevantState->getWorldRenderer().registerEvent(
					instance, "mouse-out", std::bind( &InteractionSet::modelMouseOut, this, std::placeholders::_1, std::placeholders::_2 )
				)
			);

			if( !instance->getUniform( "highlight" ) ) {
				instance->setUniform( "highlight", std::make_unique< Graphics::SceneGraph::Uniforms::HighlightUniform >( "highlight", 0.25f ) );
			}

			// TODO: GUI objects and events
		} else {
			// Remove events and uniforms, then remove instance
			for( const auto& pair : descriptor.registeredEvents ) {
				relevantState->getWorldRenderer().unregisterEvent( instance, pair.first, pair.second );
			}

			instance->setUniform( "highlight", nullptr );

			relevantState->getWorldRenderer().removeObject( instance );
			// The MODEL_REMOVED event will handle removing the interaction item
		}
	}

	void InteractionSet::associateInteraction( std::shared_ptr< Graphics::SceneGraph::Model > model, const Gameplay::Interaction& interaction ) {
		interactions[ model ].interactions.emplace( interaction );
		updateUniformsAndEvents( model );
	}

}