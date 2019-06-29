#ifndef GAMEPLAY_HOUSEHOLD_UI
#define GAMEPLAY_HOUSEHOLD_UI

#include "serializable.hpp"
#include <memory>

namespace BlueBear::State{ class HouseholdGameplayState; }
namespace BlueBear::Graphics::UserInterface::Widgets {
	class FloatingPane;
	class GridLayout;
	class Text;
}
namespace BlueBear::Gameplay::Household {

	class UserInterface : public Serializable {
		State::HouseholdGameplayState& parentState;

		std::shared_ptr< Graphics::UserInterface::Widgets::FloatingPane > controlPanel;

	public:
		UserInterface( State::HouseholdGameplayState& parentState );
		void setup();

		Json::Value save() override;
		void load( const Json::Value& data ) override;
	};

}

#endif