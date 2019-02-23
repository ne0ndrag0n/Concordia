#ifndef INTERSECTION_MAP
#define INTERSECTION_MAP

#include "tools/vector_hash.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>

namespace BlueBear::Tools::Intersection {

	struct IntersectionLineSegment {
		glm::ivec2 start;
		glm::ivec2 end;

		bool operator==( const IntersectionLineSegment& rhs ) const {
			return start == rhs.start && end == rhs.end;
		}
	};

	using IntersectionList = std::vector< IntersectionLineSegment >;
	using IntersectionMap = std::unordered_map< glm::ivec2, IntersectionList >;

	IntersectionList generateIntersectionalList( IntersectionList lineSegments );

}

namespace std {
	template <> struct hash< BlueBear::Tools::Intersection::IntersectionLineSegment > {
		size_t operator()( const BlueBear::Tools::Intersection::IntersectionLineSegment& segment ) const {
			size_t res = 17;
			res = res * 31 + hash< glm::ivec2 >()( segment.start );
			res = res * 31 + hash< glm::ivec2 >()( segment.end );
			return res;
		}
	};
}


#endif