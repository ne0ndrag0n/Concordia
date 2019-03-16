#include "graphics/scenegraph/light/sector_illuminator.hpp"
#include "graphics/texture.hpp"
#include "tools/opengl.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <algorithm>
#include <glm/gtx/string_cast.hpp>
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>

namespace BlueBear::Graphics::SceneGraph::Light {

	static glm::vec3 correctByOrigin( const glm::vec3& value, const glm::vec3& origin ) {
		return { origin.x + value.x, origin.y - value.y, value.z };
	}

	static bool segmentsIntersect( const std::pair< glm::vec3, glm::vec3 >& line1, const std::pair< glm::vec3, glm::vec3 >& line2 ) {
		// ta = (y3−y4)(x1−x3)+(x4−x3)(y1−y3)
		//      -----------------------------
		//      (x4−x3)(y1−y2)−(x1−x2)(y4−y3)

		// tb = (y1−y2)(x1−x3)+(x2−x1)(y1−y3)
		//      -----------------------------
		//      (x4−x3)(y1−y2)−(x1−x2)(y4−y3)

		// first - odd
		// second - even

		float denominator = ( ( line2.second.x - line2.first.x ) * ( line1.first.y - line1.second.y ) ) -
							( ( line1.first.x - line1.second.x ) * ( line2.second.y - line2.first.y ) );

		// collinear
		if( denominator == 0.0f ) {
			return false;
		}

	float ta_numerator = ( ( line2.first.y - line2.second.y ) * ( line1.first.x - line2.first.x ) ) +
							( ( line2.second.x - line2.first.x ) * ( line1.first.y - line2.first.y ) );

		float tb_numerator = ( ( line1.first.y - line1.second.y ) * ( line1.first.x - line2.first.x ) ) +
							( ( line1.second.x - line1.first.x ) * ( line1.first.y - line2.first.y ) );


		float ta = ta_numerator / denominator;
		float tb = tb_numerator / denominator;

		return ( ta >= 0.0f && ta <= 1.0f && tb >= 0.0f && tb <= 1.0f );
	}

	static float getPolygonMaxX( const SectorIlluminator::Sector& sector, const glm::vec3& origin ) {
		float maxX = 1.175494351e-38;

		for( const auto& lineSegment : sector.sides ) {
			maxX = std::max(
				maxX,
				std::max(
					correctByOrigin( lineSegment.first, origin ).x,
					correctByOrigin( lineSegment.second, origin ).x
				)
			);
		}

		return maxX;
	}

	void SectorIlluminator::send() {
		if( dirty ) {
			refresh();

			// Texture units 1-9 are reserved for levels.
			if( textureData.size() > 8 ) {
				Log::getInstance().warn( "SectorIlluminator::send", "Cannot send > 8 sector maps to shader!" );
			}
		}

		int resolution = std::min( ( int ) std::pow( 10, ConfigManager::getInstance().getIntValue( "sector_resolution" ) ), 100 );
		Tools::OpenGL::setUniform( "sectorResolution", ( float ) resolution );

		int item = 0;
		for( const auto& pair : levelData ) {
			Tools::OpenGL::setUniform( "sectors[" + std::to_string( item ) + "].origin", pair.second.first );
			Tools::OpenGL::setUniform( "sectors[" + std::to_string( item ) + "].dimensions", pair.second.second );

			item++;
			if( item == 8 ) {
				break;
			}
		}

		item = 0;
		for( const auto& pair : textureData ) {
			glActiveTexture( GL_TEXTURE0 + pair.second.second );
			glBindTexture( GL_TEXTURE_2D, pair.second.first->id );
			Tools::OpenGL::setUniform( "sectorMaps[" + std::to_string( item ) + "]", ( int ) pair.second.second );
			glBindTexture( GL_TEXTURE_2D, 0 );

			item++;
			if( item == 8 ) {
				break;
			}
		}

		item = 0;
		for( const Sector& sector : sectors ) {
			Tools::OpenGL::setUniform( "sectorLights[" + std::to_string( item ) + "].direction", sector.direction );
			Tools::OpenGL::setUniform( "sectorLights[" + std::to_string( item ) + "].ambient", sector.ambient );
			Tools::OpenGL::setUniform( "sectorLights[" + std::to_string( item ) + "].diffuse", sector.diffuse );
			Tools::OpenGL::setUniform( "sectorLights[" + std::to_string( item ) + "].specular", sector.specular );

			item++;
		}
	}

	/**
	 * Generate and save anything that isn't trivial to send
	 */
	void SectorIlluminator::refresh() {
		for( const auto& pair : textureData ) {
			Tools::OpenGL::returnTextureUnits( { pair.second.second } );
		}
		textureData.clear();

		for( const auto& pair : levelData ) {
			int resolution = std::min( ( int ) std::pow( 10, ConfigManager::getInstance().getIntValue( "sector_resolution" ) ), 100 );
			int height = pair.second.second.y * resolution;
			int width = pair.second.second.x * resolution;

			std::unique_ptr< float[] > array = std::make_unique< float[] >( height * width );
			const auto& constSectors = sectors;

			//tbb::parallel_for( 0, height * resolution, [ &array, &constSectors, &pair, height, width, resolution ]( int y ) {
				//tbb::parallel_for( 0, width * resolution, [ &array, &constSectors, &pair, y, height, width, resolution ]( int x ) {
			for( int y = 0; y != height; y++ ) {
				for( int x = 0; x != width; x++ ) {
					const glm::vec3 fragment = correctByOrigin( glm::vec3( x / ( float ) resolution, y / ( float ) resolution, pair.first ), pair.second.first );

					// Test fragment using point in polygon against all sectors
					int sectorIndex = 1;
					for( const Sector& sector : constSectors ) {
						// Generate needle
						std::pair< glm::vec3, glm::vec3 > needle = { fragment, glm::vec3{ getPolygonMaxX( sector, pair.second.first ) + 1.0f, fragment.y, fragment.z } };
						int fragLevel = int( fragment.z / 4 );

						// Check all sides of this sector against the needle
						unsigned int intersectionCount = 0;
						for( const auto& side : sector.sides ) {
							std::pair< glm::vec3, glm::vec3 > correctedSide = { correctByOrigin( side.first, pair.second.first ), correctByOrigin( side.second, pair.second.first ) };
							if( segmentsIntersect( needle, correctedSide ) && fragLevel == int( correctedSide.first.z / 4 ) ) {
								intersectionCount++;
							}
						}

						if( ( intersectionCount % 2 ) != 0 ) {
							// odd means IN!
							array[ ( y * width ) + x ] = sectorIndex;
							break;
						}

						sectorIndex++;
					}
				}
			}
				//} );
			//} );

			auto textureUnit = Tools::OpenGL::getTextureUnit();
			if( !textureUnit ) {
				Log::getInstance().error( "SectorIlluminator::refresh", "Unable to acquire texture unit for sector map; discarding data!" );
			} else {
				textureData[ pair.first ] = { std::make_unique< Texture >( glm::uvec2{ width, height }, array.get() ), *textureUnit };
			}
		}

		dirty = false;
	}

	void SectorIlluminator::insert( const Sector& value ) {
		dirty = true;
		sectors.emplace_back( value );
	}

	void SectorIlluminator::setLevelData( const glm::vec3& topLeft, const glm::uvec2& dimensions ) {
		dirty = true;
		levelData[ topLeft.z ] = { topLeft, dimensions };
	}

}