#include "scripting/entitykit/components/interactionset.hpp"
#include "graphics/userinterface/widgets/context_menu.hpp"
#include "graphics/userinterface/widgets/text.hpp"
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

	Json::Value InteractionSet::save() {
		// TODO
	}

    void InteractionSet::load( const Json::Value& data ) {
		// TODO
	}

	void InteractionSet::submitLuaContributions( sol::state& lua, sol::table types ) {
		types.new_usertype< InteractionSet >( "InteractionSet",
			"new", sol::no_constructor,
			"associate_interaction", &InteractionSet::associateInteraction,
			sol::base_classes, sol::bases< SystemComponent, Component >()
		);
	}

	void InteractionSet::modelMouseIn( Device::Input::Metadata event, std::shared_ptr< Graphics::SceneGraph::Model > model ) {
		Graphics::SceneGraph::Uniforms::HighlightUniform* highlighter = ( Graphics::SceneGraph::Uniforms::HighlightUniform* ) model->getUniform( "highlight" );
		highlighter->fadeTo( { 0.2f, 0.2f, 0.2f, 0.0f } );
	}

	void InteractionSet::modelMouseOut( Device::Input::Metadata event, std::shared_ptr< Graphics::SceneGraph::Model > model ) {
		Graphics::SceneGraph::Uniforms::HighlightUniform* highlighter = ( Graphics::SceneGraph::Uniforms::HighlightUniform* ) model->getUniform( "highlight" );
		highlighter->fadeTo( { 0.0f, 0.0f, 0.0f, 0.0f } );
	}

	void InteractionSet::modelMouseDown( Device::Input::Metadata event, std::shared_ptr< Graphics::SceneGraph::Model > model ) {
		Log::getInstance().debug( "Got", "Here" );
		//relevantState->getGuiComponent().addElement( getMenuWidget( event.mouseLocation, interactions[ model ].interactions ) );
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

			descriptor.registeredEvents.emplace_back(
				"mouse-down",
				relevantState->getWorldRenderer().registerEvent(
					instance, "mouse-down", std::bind( &InteractionSet::modelMouseDown, this, std::placeholders::_1, std::placeholders::_2 )
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

	std::shared_ptr< Graphics::UserInterface::Widgets::ContextMenu > InteractionSet::getMenuWidget( const glm::ivec2& position, const std::vector< Gameplay::Interaction >& interactions ) {
		auto menu = Graphics::UserInterface::Widgets::ContextMenu::create( "", { "__contextmenu" } );

		for( const auto& interaction : interactions ) {
			menu->addChild( Graphics::UserInterface::Widgets::Text::create( "", { "__contextmenu_item" }, interaction.label ), false );
		}

		menu->getPropertyList().set< int >( "left", position.x );
		menu->getPropertyList().set< int >( "top", position.y );

		return menu;
	}

	void InteractionSet::associateInteraction( std::shared_ptr< Graphics::SceneGraph::Model > model, const Gameplay::Interaction& interaction ) {
		interactions[ model ].interactions.emplace_back( interaction );
		updateUniformsAndEvents( model );
	}

}