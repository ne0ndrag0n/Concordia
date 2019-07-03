#ifndef BB_GEOM_METHODS
#define BB_GEOM_METHODS

#include "geometry/ray.hpp"
#include "geometry/triangle.hpp"
#include "geometry/aabb.hpp"
#include "geometry/linesegment.hpp"
#include "geometry/polygon2d.hpp"
#include <optional>
#include <vector>

namespace BlueBear::Geometry {

	std::optional< glm::vec3 > getIntersectionPoint( const Ray& ray, const Triangle& triangle );
	std::optional< glm::vec3 > getIntersectionPoint( const Ray& ray, const AABB& aabb );

	bool segmentsIntersect( const LineSegment< glm::vec2 >& line1, const LineSegment< glm::vec2 >& line2 );

	float polygonArea( const Polygon2D& polygon );
	bool polygonClockwise( const Polygon2D& polygon );
	Polygon2D polygonReverse( const Polygon2D& polygon );
}

#endif