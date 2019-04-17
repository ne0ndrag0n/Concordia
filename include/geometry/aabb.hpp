#ifndef CONCORDIA_AABB
#define CONCORDIA_AABB

#include <glm/glm.hpp>

namespace BlueBear::Geometry {

	struct AABB {
		glm::vec3 minima;
		glm::vec3 maxima;
	};

}

#endif