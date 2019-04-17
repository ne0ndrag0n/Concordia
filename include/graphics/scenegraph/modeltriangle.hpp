#ifndef SG_MODEL_TRIANGLE
#define SG_MODEL_TRIANGLE

#include "geometry/triangle.hpp"
#include <glm/glm.hpp>
#include <utility>

namespace BlueBear::Graphics::SceneGraph {

	using ModelTriangle = std::pair< Geometry::Triangle, glm::mat4 >;

}

#endif