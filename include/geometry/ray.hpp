#ifndef BB_RAY
#define BB_RAY

#include <glm/glm.hpp>

namespace BlueBear::Geometry {

	struct Ray {
		glm::vec3 origin;
		glm::vec3 direction;
	};

}

#endif