#ifndef GAMEPLAY_HOUSEHOLD_UI
#define GAMEPLAY_HOUSEHOLD_UI

#include <memory>

namespace BlueBear::Graphics::UserInterface { class Element; }
namespace BlueBear::Gameplay::Household {

	class UserInterface {
		std::shared_ptr< Graphics::UserInterface::Element > sidePane;
		std::shared_ptr< Graphics::UserInterface::Element > controlPanel;

		void setupControlPanel();

	public:
		UserInterface();
	};

}

#endif