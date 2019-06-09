#ifndef CONCORDIA_GUI_UTILITY
#define CONCORDIA_GUI_UTILITY

#include <glm/glm.hpp>
#include <memory>

namespace BlueBear::Graphics::UserInterface {
	class Element;

	struct Utility {
		static bool valueIsLiteral( int r );
		static glm::uvec2 getFinalRequisition( const std::shared_ptr< Element >& prospect );
	};

}

#endif