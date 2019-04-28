#include "scripting/entitykit/components/interactionset.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include "graphics/scenegraph/uniforms/highlight_uniform.hpp"
#include "scripting/entitykit/entity.hpp"
#include "scripting/entitykit/components/componentcast.hpp"
#include "containers/visitor.hpp"
#include "log.hpp"

namespace BlueBear::Scripting::EntityKit::Components {

	InteractionSet::InteractionSet() : SystemComponent::SystemComponent( "system.component.interaction_set" ) {}

	void InteractionSet::refresh() {
		if( modelManager ) {
			// Remove uniforms and events from previous set (they will be restored when regenerated)
			// If model was removed since last update, unregisterEvent will be ignored and model will be deleted when set is cleared
			for( const EventDescriptor& descriptor : previousItems ) {
				for( const auto& pair : descriptor.registeredEvents ) {
					ModelManager::worldRenderer->unregisterEvent( descriptor.model, pair.first, pair.second );
				}

				descriptor.model->removeUniform( "highlight" );
			}

			previousItems.clear();

			// Get models currently registered
			auto models = modelManager->getModels();
			for( auto pair : models ) {
				// Determine if this item has any interactions
				auto it = interactions.find( pair.first );
				if( it != interactions.end() ) {
					// Attach mouse-in and mouse-out events
					int mouseInEvent = ModelManager::worldRenderer->registerEvent(
						pair.second, "mouse-in", std::bind( &InteractionSet::modelMouseIn, this, std::placeholders::_1, std::placeholders::_2 )
					);

					int mouseOutEvent = ModelManager::worldRenderer->registerEvent(
						pair.second, "mouse-out", std::bind( &InteractionSet::modelMouseOut, this, std::placeholders::_1, std::placeholders::_2 )
					);

					// Attach higlight uniform
					pair.second->setUniform( "highlight", std::make_unique< Graphics::SceneGraph::Uniforms::HighlightUniform >( "highlight", 0.25f ) );

					// Log this as a "previous item"
					previousItems.emplace( EventDescriptor{
						pair.second,
						{
							{ "mouse-in", mouseInEvent },
							{ "mouse-out", mouseOutEvent }
						}
					} );

					// TODO: Create GUI elements for interactions
					// TODO: Create mouse-down event for showing GUI element
				}
			}
		} else {
			Log::getInstance().debug( "InteractionSet::refresh", "No model manager attached" );
		}
	}


	void InteractionSet::modelMouseIn( Device::Input::Metadata event, std::shared_ptr< Graphics::SceneGraph::Model > model ) {
		Graphics::SceneGraph::Uniforms::HighlightUniform* highlighter = ( Graphics::SceneGraph::Uniforms::HighlightUniform* ) model->getUniform( "highlight" );
		highlighter->fadeTo( { 0.2f, 0.2f, 0.2f, 0.0f } );
	}

	void InteractionSet::modelMouseOut( Device::Input::Metadata event, std::shared_ptr< Graphics::SceneGraph::Model > model ) {
		Graphics::SceneGraph::Uniforms::HighlightUniform* highlighter = ( Graphics::SceneGraph::Uniforms::HighlightUniform* ) model->getUniform( "highlight" );
		highlighter->fadeTo( { 0.0f, 0.0f, 0.0f, 0.0f } );
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