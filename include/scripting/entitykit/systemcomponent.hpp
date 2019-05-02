#ifndef ENTITYKIT_SYSTEM_COMPONENT
#define ENTITYKIT_SYSTEM_COMPONENT

#include "scripting/entitykit/component.hpp"

namespace BlueBear::State{ class HouseholdGameplayState; }
namespace BlueBear::Scripting::EntityKit {

	class SystemComponent : public Component {
	public:
		using Component::Component;

		static BlueBear::State::HouseholdGameplayState* relevantState;
	};

}

#endif