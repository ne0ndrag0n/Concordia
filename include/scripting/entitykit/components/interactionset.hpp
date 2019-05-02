#ifndef INTERACTION_SET_COMPONENT
#define INTERACTION_SET_COMPONENT

#include "scripting/entitykit/components/modelmanager.hpp"
#include "scripting/entitykit/systemcomponent.hpp"
#include "graphics/scenegraph/model.hpp"
#include "gameplay/interaction.hpp"
#include "device/input/input.hpp"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <sol.hpp>
#include <memory>
#include <set>

namespace BlueBear::Graphics::SceneGraph { class Model; }
namespace BlueBear::Scripting::EntityKit::Components {

	class InteractionSet : public SystemComponent {
		struct EventDescriptor {
			std::set< Gameplay::Interaction > interactions;
			std::vector< std::pair< std::string, unsigned int > > registeredEvents;
		};

		std::map< std::shared_ptr< Graphics::SceneGraph::Model >, EventDescriptor > interactions;

		void modelMouseIn( Device::Input::Metadata event, std::shared_ptr< Graphics::SceneGraph::Model > model );
        void modelMouseOut( Device::Input::Metadata event, std::shared_ptr< Graphics::SceneGraph::Model > model );
		void modelRemoved( std::shared_ptr< Graphics::SceneGraph::Model > model );

		void updateUniformsAndEvents( std::shared_ptr< Graphics::SceneGraph::Model > instance );

	public:
		InteractionSet();
		~InteractionSet();

		Json::Value save() override;
		void load( const Json::Value& data ) override;

		static void submitLuaContributions( sol::state& lua, sol::table types );

		void associateInteraction( std::shared_ptr< Graphics::SceneGraph::Model > model, const Gameplay::Interaction& interaction );
	};

}

#endif