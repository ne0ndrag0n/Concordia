#ifndef ENTITYKIT_SYSTEM_COMPONENT
#define ENTITYKIT_SYSTEM_COMPONENT

#include "scripting/entitykit/component.hpp"

namespace BlueBear::Scripting::EntityKit {

	class SystemComponent : public Component {
	public:
		using Component::Component;
	};

}

#endif