#include "tools/intersection_map.hpp"
#include <unordered_set>

namespace BlueBear::Tools::Intersection {

	IntersectionList generateIntersectionalList( IntersectionList lineSegments ) {

		// Step 1: Get all intersections for each line segment
		std::unordered_map< const IntersectionLineSegment*, std::unordered_set< glm::ivec2 > > crossedVertices;

		for( const auto& lineSegment : lineSegments ) {
			glm::ivec2 direction = glm::ivec2( glm::normalize( glm::vec2( lineSegment.end ) - glm::vec2( lineSegment.start ) ) );
			glm::ivec2 cursor = lineSegment.start;

			while( cursor.x <= lineSegment.end.x && cursor.y <= lineSegment.end.y ) {
				// For every floor vertex this touches, check every line that isn't this one for an intersection
				// Lazy, shitty, but I can't be arsed to decode mathspeak in academic papers
				// Hide this garbage behind a thread/future and design the user experience around paitence
				for( const auto& otherLineSegment : lineSegments ) {
					if( &lineSegment != &otherLineSegment ) {
						glm::ivec2 otherDirection = glm::ivec2( glm::normalize( glm::vec2( otherLineSegment.end ) - glm::vec2( otherLineSegment.start ) ) );
						glm::ivec2 otherCursor = otherLineSegment.start;

						while( otherCursor.x <= otherLineSegment.end.x && otherCursor.y <= otherLineSegment.end.y ) {
							if( cursor == otherCursor ) {
								crossedVertices[ &lineSegment ].emplace( cursor );
							}

							otherCursor += otherDirection;
						}
					}
				}


				cursor += direction;
			}

		}

		// TODO

		return lineSegments;
	}

}