#include "graphics/scenegraph/light/sector_illuminator.hpp"
#include "tools/opengl.hpp"
#include "log.hpp"
#include <algorithm>
#include <glm/gtx/string_cast.hpp>

namespace BlueBear::Graphics::SceneGraph::Light {

	SectorIlluminator::SectorIlluminator( const std::vector< Sector >& sectors ) : Illuminator::Illuminator() {
		updateSectors( sectors );
	}

	static glm::vec3 correctByOrigin( const glm::vec3& value, const glm::vec3& origin ) {
		return { origin.x + value.x, origin.y - value.y, value.z };
	}

	void SectorIlluminator::send() {
		std::vector< std::pair< glm::vec3, glm::vec3 > > lineSegments;

		Tools::OpenGL::setUniform( "sectorIlluminator.numSectors", ( unsigned int ) sectors.size() );
		int sectorIndex = 0;
		for( const Sector& sector : sectors ) {
			Tools::OpenGL::setUniform( "sectorIlluminator.sectors[" + std::to_string( sectorIndex ) + "].light.direction", sector.direction );
			Tools::OpenGL::setUniform( "sectorIlluminator.sectors[" + std::to_string( sectorIndex ) + "].light.ambient", sector.ambient );
			Tools::OpenGL::setUniform( "sectorIlluminator.sectors[" + std::to_string( sectorIndex ) + "].light.diffuse", sector.diffuse );
			Tools::OpenGL::setUniform( "sectorIlluminator.sectors[" + std::to_string( sectorIndex ) + "].light.specular", sector.specular );

			Tools::OpenGL::setUniform( "sectorIlluminator.sectors[" + std::to_string( sectorIndex ) + "].polygon.numSides", ( unsigned int ) sector.sides.size() );
			int sidesIndex = 0;
			for( auto lineSegment : sector.sides ) {
				const glm::vec3& origin = origins[ lineSegment.first.z ];
				lineSegment.first = correctByOrigin( lineSegment.first, origin );
				lineSegment.second = correctByOrigin( lineSegment.second, origin );

				auto it = std::find_if( lineSegments.begin(), lineSegments.end(), [ &lineSegment ]( const std::pair< glm::vec3, glm::vec3 >& value ) {
					return lineSegment.first == value.first &&
						   lineSegment.second == value.second;
				} );

				unsigned int lineSegmentIndex;
				if( it == lineSegments.end() ) {
					// Line segment needs to be added
					lineSegments.emplace_back( lineSegment );
					lineSegmentIndex = lineSegments.size() - 1;
				} else {
					// Line segment already exists
					lineSegmentIndex = std::distance( lineSegments.begin(), it );
				}

				Tools::OpenGL::setUniform( "sectorIlluminator.sectors[" + std::to_string( sectorIndex ) + "].polygon.sides[" + std::to_string( sidesIndex ) + "]", ( unsigned int ) lineSegmentIndex );
				sidesIndex++;
			}

			sectorIndex++;
		}

		int lineSegmentsIndex = 0;
		for( const auto& lineSegment : lineSegments ) {
			Tools::OpenGL::setUniform( "sectorIlluminator.lineSegments[" + std::to_string( lineSegmentsIndex ) + "].from", lineSegment.first );
			Tools::OpenGL::setUniform( "sectorIlluminator.lineSegments[" + std::to_string( lineSegmentsIndex ) + "].to", lineSegment.second );

			lineSegmentsIndex++;
		}
	}

	void SectorIlluminator::updateSectors( const std::vector< Sector >& sectors ) {
		this->sectors = sectors;
	}

	void SectorIlluminator::insert( const Sector& value ) {
		sectors.emplace_back( value );
	}

	void SectorIlluminator::setOrigin( float level, const glm::vec3& topLeft ) {
		origins[ level ] = topLeft;
	}

	SectorIlluminator::Sector* SectorIlluminator::getSector() {
		// TODO point in polygon
		return nullptr;
	}

}