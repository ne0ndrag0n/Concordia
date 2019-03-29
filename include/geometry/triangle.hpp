#ifndef BB_BASIC_TRIANGLE
#define BB_BASIC_TRIANGLE

#include <glm/glm.hpp>
#include <array>

namespace BlueBear::Geometry {

	// Generic triangle type
	using Triangle = std::array< glm::vec3, 3 >;

}


#endif