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

	/**
	 * mit licenced ray-AABB intersection
	 * https://github.com/stackgl/ray-aabb-intersection/blob/master/index.js
	 */
	std::optional< glm::vec3 > getIntersectionPoint( const Ray& ray, const AABB& aabb ) {
		double low = std::numeric_limits< double >::lowest();
		double high = std::numeric_limits< double >::max();

		for( int i = 0; i != 3; i++ ) {
			double dimLow = ( aabb.minima[ i ] - ray.origin[ i ] ) / ray.direction[ i ];
			double dimHigh = ( aabb.maxima[ i ] - ray.origin[ i ] ) / ray.direction[ i ];

			if( dimLow > dimHigh ) {
				double tmp = dimLow;
				dimLow = dimHigh;
				dimHigh = tmp;
			}

			if( dimHigh < low || dimLow > high ) {
				return {};
			}

			if( dimLow > low ) { low = dimLow; }
			if( dimHigh < high ) { high = dimHigh; }
		}

		if( low > high ) {
			return {};
		}

		double distance = low;
		// At this point there must be an intersection
		return glm::vec3{
			ray.origin.x + ray.direction.x * distance,
			ray.origin.y + ray.direction.y * distance,
			ray.origin.z + ray.direction.z * distance
		};
	}

}