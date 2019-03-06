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
		if( memo.dirty ) {
			refresh();
		}

		sendMemoized();
	}

	/**
	 * Generate and save anything that isn't trivial to send
	 */
	void SectorIlluminator::refresh() {
		memo.dirty = false;
		memo.lineSegments.clear();
		memo.polygonSides.clear();

		int sectorIndex = 0;
		for( const Sector& sector : sectors ) {

			int sidesIndex = 0;
			for( auto lineSegment : sector.sides ) {
				const glm::vec3& origin = origins[ lineSegment.first.z ];
				lineSegment.first = correctByOrigin( lineSegment.first, origin );
				lineSegment.second = correctByOrigin( lineSegment.second, origin );

				auto it = std::find_if( memo.lineSegments.begin(), memo.lineSegments.end(), [ &lineSegment ]( const std::pair< glm::vec3, glm::vec3 >& value ) {
					return lineSegment.first == value.first &&
						   lineSegment.second == value.second;
				} );

				unsigned int lineSegmentIndex;
				if( it == memo.lineSegments.end() ) {
					// Line segment needs to be added
					memo.lineSegments.emplace_back( lineSegment );
					lineSegmentIndex = memo.lineSegments.size() - 1;
				} else {
					// Line segment already exists
					lineSegmentIndex = std::distance( memo.lineSegments.begin(), it );
				}

				memo.polygonSides.emplace_back( sectorIndex, sidesIndex, lineSegmentIndex );
				sidesIndex++;
			}

			sectorIndex++;
		}
	}

	/**
	 * Send everything that's trivial to send
	 */
	void SectorIlluminator::sendMemoized() {
		Tools::OpenGL::setUniform( "sectorIlluminator.numSectors", ( unsigned int ) sectors.size() );
		int sectorIndex = 0;
		for( const Sector& sector : sectors ) {
			Tools::OpenGL::setUniform( "sectorIlluminator.sectors[" + std::to_string( sectorIndex ) + "].light.direction", sector.direction );
			Tools::OpenGL::setUniform( "sectorIlluminator.sectors[" + std::to_string( sectorIndex ) + "].light.ambient", sector.ambient );
			Tools::OpenGL::setUniform( "sectorIlluminator.sectors[" + std::to_string( sectorIndex ) + "].light.diffuse", sector.diffuse );
			Tools::OpenGL::setUniform( "sectorIlluminator.sectors[" + std::to_string( sectorIndex ) + "].light.specular", sector.specular );
			Tools::OpenGL::setUniform( "sectorIlluminator.sectors[" + std::to_string( sectorIndex ) + "].polygon.numSides", ( unsigned int ) sector.sides.size() );
			sectorIndex++;
		}

		for( auto [ sectorIndex, sidesIndex, lineSegmentIndex ] : memo.polygonSides ) {
			Tools::OpenGL::setUniform( "sectorIlluminator.sectors[" + std::to_string( sectorIndex ) + "].polygon.sides[" + std::to_string( sidesIndex ) + "]", ( unsigned int ) lineSegmentIndex );
		}

		int lineSegmentsIndex = 0;
		for( const auto& lineSegment : memo.lineSegments ) {
			Tools::OpenGL::setUniform( "sectorIlluminator.lineSegments[" + std::to_string( lineSegmentsIndex ) + "].from", lineSegment.first );
			Tools::OpenGL::setUniform( "sectorIlluminator.lineSegments[" + std::to_string( lineSegmentsIndex ) + "].to", lineSegment.second );
			lineSegmentsIndex++;
		}
	}

	void SectorIlluminator::updateSectors( const std::vector< Sector >& sectors ) {
		memo.dirty = true;
		this->sectors = sectors;
	}

	void SectorIlluminator::insert( const Sector& value ) {
		memo.dirty = true;
		sectors.emplace_back( value );
	}

	void SectorIlluminator::setOrigin( float level, const glm::vec3& topLeft ) {
		memo.dirty = true;
		origins[ level ] = topLeft;
	}

}