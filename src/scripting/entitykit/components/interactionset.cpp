#include "scripting/entitykit/components/interactionset.hpp"
#include "graphics/userinterface/widgets/context_menu.hpp"
#include "graphics/userinterface/widgets/text.hpp"
#include "device/display/adapter/component/worldrenderer.hpp"
#include "graphics/scenegraph/uniforms/highlight_uniform.hpp"
#include "scripting/entitykit/entity.hpp"
#include "scripting/entitykit/components/componentcast.hpp"
#include "scripting/coreengine.hpp"
#include "state/householdgameplaystate.hpp"
#include "containers/visitor.hpp"
#include "log.hpp"
#include <functional>

namespace BlueBear::Scripting::EntityKit::Components {

	InteractionSet::InteractionSet() : SystemComponent::SystemComponent( "system.component.interaction_set" ) {
		relevantState->getWorldRenderer().MODEL_REMOVED.listen( this, std::bind( &InteractionSet::modelRemoved, this, std::placeholders::_1 ) );
		relevantState->getGuiComponent().GUI_OBJECT_MOUSE_DOWN.listen( this, std::bind( &InteractionSet::checkClickOut, this, std::placeholders::_1 ) );
	}

	InteractionSet::~InteractionSet() {
		relevantState->getWorldRenderer().MODEL_REMOVED.stopListening( this );
		relevantState->getGuiComponent().GUI_OBJECT_MOUSE_DOWN.stopListening( this );
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

	void InteractionSet::checkClickOut( std::shared_ptr< Graphics::UserInterface::Element > selectedElement ) {
		if( menu && selectedElement ) {
			// This is gonna have to be more involved if we do image + text, etc
			auto items = menu->getChildren();
			items.emplace_back( menu );

			for( const auto& item : items ) {
				if( item == selectedElement ) {
					// Bail out
					return;
				}
			}

			// If we get here then selectedElement was something totally unrelated to the menu
			// Close + remove it
			removeMenu();
		}
	}

	void InteractionSet::removeMenu() {
		relevantState->getGuiComponent().removeElement( menu );
		menu = nullptr;
		menuModel = nullptr;
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
		if( !menu ) {
			menu = getMenuWidget( interactions[ model ].interactions );
			relevantState->getGuiComponent().addElement( menu );

			menu->getPropertyList().set< int >( "left", event.mouseLocation.x );
			menu->getPropertyList().set< int >( "top", event.mouseLocation.y );
			menu->getPropertyList().set< Graphics::UserInterface::Placement >( "placement", Graphics::UserInterface::Placement::FREE );

			menuModel = model;
		}
	}

	void InteractionSet::modelRemoved( std::shared_ptr< Graphics::SceneGraph::Model > model ) {
		// Perform a simple drop of all interactions for this model
		interactions.erase( model );

		if( menuModel == model ) {
			removeMenu();
		}
	}

	void InteractionSet::updateUniformsAndEvents( std::shared_ptr< Graphics::SceneGraph::Model > instance ) {
		EventDescriptor& descriptor = interactions[ instance ];

		if( !descriptor.interactions.empty() ) {
			// Ensure events and uniforms are set up
			if( !descriptor.mouseInEvent ) {
				descriptor.mouseInEvent = relevantState->getWorldRenderer().registerEvent(
					instance, "mouse-in", std::bind( &InteractionSet::modelMouseIn, this, std::placeholders::_1, std::placeholders::_2 )
				);
			}

			if( !descriptor.mouseOutEvent ) {
				descriptor.mouseOutEvent = relevantState->getWorldRenderer().registerEvent(
					instance, "mouse-out", std::bind( &InteractionSet::modelMouseOut, this, std::placeholders::_1, std::placeholders::_2 )
				);
			}

			if( !descriptor.mouseDownEvent ) {
				descriptor.mouseDownEvent = relevantState->getWorldRenderer().registerEvent(
					instance, "mouse-down", std::bind( &InteractionSet::modelMouseDown, this, std::placeholders::_1, std::placeholders::_2 )
				);
			}

			if( !instance->getUniform( "highlight" ) ) {
				instance->setUniform( "highlight", std::make_unique< Graphics::SceneGraph::Uniforms::HighlightUniform >( "highlight", 0.25f ) );
			}
		} else {
			// Remove events and uniforms, then remove instance
			if( descriptor.mouseInEvent ) {
				relevantState->getWorldRenderer().unregisterEvent( instance, "mouse-in", *descriptor.mouseInEvent );
			}

			if( descriptor.mouseOutEvent ) {
				relevantState->getWorldRenderer().unregisterEvent( instance, "mouse-out", *descriptor.mouseOutEvent );
			}

			if( descriptor.mouseDownEvent ) {
				relevantState->getWorldRenderer().unregisterEvent( instance, "mouse-down", *descriptor.mouseDownEvent );
			}

			instance->setUniform( "highlight", nullptr );

			relevantState->getWorldRenderer().removeObject( instance );
			// The MODEL_REMOVED event will handle removing the interaction item
		}
	}

	std::shared_ptr< Graphics::UserInterface::Widgets::ContextMenu > InteractionSet::getMenuWidget( const std::vector< Gameplay::Interaction >& interactions ) {
		auto menu = Graphics::UserInterface::Widgets::ContextMenu::create( "", { "__contextmenu" } );

		for( const auto& interaction : interactions ) {
			auto text = Graphics::UserInterface::Widgets::Text::create( "", { "__contextmenu_item" }, interaction.label );
			text->getEventBundle().registerInputEvent( "mouse-up", [ this, callback = interaction.callback ]( const Device::Input::Metadata& metadata ) {
				relevantState->getEngine().setTimeout( 1, callback );
				removeMenu();
			} );
			menu->addChild( text, false );
		}

		return menu;
	}

	void InteractionSet::associateInteraction( std::shared_ptr< Graphics::SceneGraph::Model > model, const Gameplay::Interaction& interaction ) {
		interactions[ model ].interactions.emplace_back( interaction );
		updateUniformsAndEvents( model );
	}

}