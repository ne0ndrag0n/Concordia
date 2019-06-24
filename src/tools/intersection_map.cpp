#include "tools/intersection_map.hpp"
#include <unordered_set>
#include <map>

#include "log.hpp"
#include <glm/gtx/string_cast.hpp>

namespace BlueBear::Tools::Intersection {

	IntersectionList generateIntersectionalList( IntersectionList lineSegments, const glm::ivec2& totalDimensions ) {

		// Step 1: Get all intersections for each line segment
		auto comp = [ &totalDimensions ]( const glm::ivec2& left, const glm::ivec2& right ) -> bool {
			return ( ( left.y * totalDimensions.x ) + left.x ) < ( ( right.y * totalDimensions.x ) + right.x );
		};
		std::map< glm::ivec2, std::unordered_set< IntersectionLineSegment* >, decltype( comp ) > crossedVertices( comp );

		for( auto& lineSegment : lineSegments ) {
			glm::ivec2 direction = glm::ivec2( glm::normalize( glm::vec2( lineSegment.end ) - glm::vec2( lineSegment.start ) ) );
			glm::ivec2 cursor = lineSegment.start;

			while( cursor.x <= lineSegment.end.x && cursor.y <= lineSegment.end.y ) {
				// For every floor vertex this touches, check every line that isn't this one for an intersection
				// Lazy, shitty, but I can't be arsed to decode mathspeak in academic papers
				// Hide this garbage behind a thread/future and design the user experience around paitence
				for( auto& otherLineSegment : lineSegments ) {
					if( &lineSegment != &otherLineSegment ) {
						glm::ivec2 otherDirection = glm::ivec2( glm::normalize( glm::vec2( otherLineSegment.end ) - glm::vec2( otherLineSegment.start ) ) );
						glm::ivec2 otherCursor = otherLineSegment.start;

						while( otherCursor.x <= otherLineSegment.end.x && otherCursor.y <= otherLineSegment.end.y ) {
							if( cursor == otherCursor ) {
								crossedVertices[ cursor ].emplace( &lineSegment );
							}

							otherCursor += otherDirection;
						}
					}
				}


				cursor += direction;
			}

		}

		// Step 2: Subdivide lines at intersection points, top-to-bottom, left-to-right
		for( auto& pair : crossedVertices ) {
			for( IntersectionLineSegment* lineSegment : pair.second ) {
				// Do not subdivide if the point is identical to either start or end
				if( pair.first != lineSegment->start && pair.first != lineSegment->end ) {
					// Create new line segment ranging from old.start to pair.first
					IntersectionLineSegment newLineSegment{ lineSegment->start, pair.first };
					// Set old segment's new start to pair.first
					lineSegment->start = pair.first;
					// Insert new line segment
					lineSegments.emplace_back( std::move( newLineSegment ) );
				}
			}
		}

		return lineSegments;
	}

}