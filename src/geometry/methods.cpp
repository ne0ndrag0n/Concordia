#include "geometry/methods.hpp"

namespace BlueBear::Geometry {

	/**
	 * mit licenced moller-trumbore algorithm
	 * https://github.com/substack/ray-triangle-intersection/blob/master/index.js
	 */
	std::optional< glm::vec3 > getIntersectionPoint( const Ray& ray, const Triangle& triangle ) {
		glm::vec3 edge1 = triangle[ 1 ] - triangle[ 0 ];
		glm::vec3 edge2 = triangle[ 2 ] - triangle[ 0 ];

		glm::vec3 pvec = glm::cross( ray.direction, edge2 );
		float determinant = glm::dot( edge1, pvec );

		if( determinant < 0.000001f ) {
			return {};
		}
		glm::vec3 tvec = ray.origin - triangle[ 0 ];
		float u = glm::dot( tvec, pvec );
		if( u < 0 || u > determinant ) {
			return {};
		}
		glm::vec3 qvec = glm::cross( tvec, edge1 );
		float v = glm::dot( ray.direction, qvec );
		if( v < 0 || ( u + v ) > determinant ) {
			return {};
		}

		float t = glm::dot( edge2, qvec ) / determinant;
		return ray.origin + t * ray.direction;
	}

}