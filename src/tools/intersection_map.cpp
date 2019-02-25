#include "tools/intersection_map.hpp"
#include <unordered_set>
#include <map>

#include "log.hpp"
#include <glm/gtx/string_cast.hpp>

namespace BlueBear::Tools::Intersection {

	// cheap hack to get comparator to take direction into account
	static float getDirectionFactor( const glm::ivec2 direction ) {
		if( direction == glm::ivec2{ 0, -1 } ) {
			return 1.0f;
		} else if( direction == glm::ivec2{ 1, -1 } ) {
			return 2.0f;
		} else if( direction == glm::ivec2{ 1, 0 } ) {
			return 3.0f;
		} else if( direction == glm::ivec2{ 1, 1 } ) {
			return 4.0f;
		} else if( direction == glm::ivec2{ 0, 1 } ) {
			return 5.0f;
		} else if( direction == glm::ivec2{ -1, 1 } ) {
			return 6.0f;
		} else if( direction == glm::ivec2{ -1, 0 } ) {
			return 7.0f;
		} else if( direction == glm::ivec2{ -1, -1 } ) {
			return 8.0f;
		}
	}

	IntersectionList generateIntersectionalList( IntersectionList lineSegments ) {

		// Step 1: Get all intersections for each line segment
		struct comparator {
			bool operator()( const glm::ivec2& left, const glm::ivec2& right ) const {
				return
					glm::distance( glm::vec2{ 0, 0 }, glm::vec2( left ) ) + getDirectionFactor( glm::normalize( glm::vec2( left ) ) )
						<
					glm::distance( glm::vec2{ 0, 0 }, glm::vec2( right ) ) + getDirectionFactor( glm::normalize( glm::vec2( right ) ) );
			}
		};
		std::map< glm::ivec2, std::unordered_set< IntersectionLineSegment* >, comparator > crossedVertices;

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