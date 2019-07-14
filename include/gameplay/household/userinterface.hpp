#ifndef GAMEPLAY_HOUSEHOLD_UI
#define GAMEPLAY_HOUSEHOLD_UI

#include "serializable.hpp"
#include <memory>

namespace BlueBear::State{ class HouseholdGameplayState; }
namespace BlueBear::Graphics::UserInterface{ class Element; }
namespace BlueBear::Graphics::UserInterface::Widgets {
	class FloatingPane;
	class GridLayout;
	class Text;
	class Button;
}
namespace BlueBear::Gameplay::Household {

	class UserInterface : public Serializable {
		State::HouseholdGameplayState& parentState;

		std::shared_ptr< Graphics::UserInterface::Widgets::FloatingPane > controlPanel;
		std::shared_ptr< Graphics::UserInterface::Widgets::Button > rotateRight;
		std::shared_ptr< Graphics::UserInterface::Widgets::Button > rotateLeft;
		std::shared_ptr< Graphics::UserInterface::Widgets::Button > zoomIn;
		std::shared_ptr< Graphics::UserInterface::Widgets::Button > zoomOut;

		std::shared_ptr< Graphics::UserInterface::Widgets::Button > noWalls;
		std::shared_ptr< Graphics::UserInterface::Widgets::Button > cutaway;
		std::shared_ptr< Graphics::UserInterface::Widgets::Button > wallsUp;


	public:
		UserInterface( State::HouseholdGameplayState& parentState );
		void setup();

		Json::Value save() override;
		void load( const Json::Value& data ) override;
	};

}

#endif