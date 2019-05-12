#ifndef BB_GEOM_METHODS
#define BB_GEOM_METHODS

#include "geometry/ray.hpp"
#include "geometry/triangle.hpp"
#include "geometry/aabb.hpp"
#include "geometry/linesegment.hpp"
#include <optional>

namespace BlueBear::Geometry {

	std::optional< glm::vec3 > getIntersectionPoint( const Ray& ray, const Triangle& triangle );
	std::optional< glm::vec3 > getIntersectionPoint( const Ray& ray, const AABB& aabb );

	bool segmentsIntersect( const LineSegment< glm::vec2 >& line1, const LineSegment< glm::vec2 >& line2 );

}

#endif