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

	bool segmentsIntersect( const LineSegment< glm::vec2 >& line1, const LineSegment< glm::vec2 >& line2 ) {
		// ta = (y3−y4)(x1−x3)+(x4−x3)(y1−y3)
		//      -----------------------------
		//      (x4−x3)(y1−y2)−(x1−x2)(y4−y3)

		// tb = (y1−y2)(x1−x3)+(x2−x1)(y1−y3)
		//      -----------------------------
		//      (x4−x3)(y1−y2)−(x1−x2)(y4−y3)

		// first - odd
		// second - even

		float denominator = ( ( line2.to.x - line2.from.x ) * ( line1.from.y - line1.to.y ) ) -
							( ( line1.from.x - line1.to.x ) * ( line2.to.y - line2.from.y ) );

		// collinear
		if( denominator == 0.0f ) {
			return false;
		}

		float ta_numerator = ( ( line2.from.y - line2.to.y ) * ( line1.from.x - line2.from.x ) ) +
							 ( ( line2.to.x - line2.from.x ) * ( line1.from.y - line2.from.y ) );

		float tb_numerator = ( ( line1.from.y - line1.to.y ) * ( line1.from.x - line2.from.x ) ) +
							 ( ( line1.to.x - line1.from.x ) * ( line1.from.y - line2.from.y ) );

		float ta = ta_numerator / denominator;
		float tb = tb_numerator / denominator;

		return ( ta >= 0.0f && ta <= 1.0f && tb >= 0.0f && tb <= 1.0f );
	}

	/**
	 * mit-licenced polygon area
	 * https://github.com/tmpvar/polygon.js
	 */
	float polygonArea( const Polygon2D& polygon ) {
		float area = 0.0f;

		if( polygon.empty() ) {
			return 0.0f;
		}

		const glm::vec2& first = polygon[ 0 ];
		for( size_t i = 0; i != polygon.size(); i++ ) {
			if( i < 2 ) {
				continue;
			}

			const glm::vec2& edge1 = first - polygon[ i ];
			const glm::vec2& edge2 = first - polygon[ i - 1 ];

			area += ( ( edge1.x * edge2.y ) - ( edge1.y * edge2.x ) );
		}

		return area / 2.0f;
	}

	bool polygonClockwise( const Polygon2D& polygon ) {
		return polygonArea( polygon ) > 0.0f;
	}

	Polygon2D polygonReverse( const Polygon2D& polygon ) {
		Polygon2D result;

		for( auto it = polygon.rbegin(); it != polygon.rend(); ++it ) {
			result.emplace_back( *it );
		}

		return result;
	}

}