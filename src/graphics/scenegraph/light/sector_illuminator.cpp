#include "graphics/scenegraph/light/sector_illuminator.hpp"
#include "graphics/texture.hpp"
#include "tools/opengl.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <algorithm>
#include <limits>
#include <chrono>
#include <glm/gtx/string_cast.hpp>
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>

namespace BlueBear::Graphics::SceneGraph::Light {

	SectorIlluminator::~SectorIlluminator() {
		for( const auto& pair : textureData ) {
			if( pair.textureUnit ) {
				Tools::OpenGL::returnTextureUnits( { *pair.textureUnit } );
			}
		}
	}

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

	const SectorIlluminator::ShaderUniformSet& SectorIlluminator::getShaderUniformSet( const Shader* address ) {
		auto it = shaderUniformSets.find( address );
		if( it != shaderUniformSets.end() ) {
			return it->second;
		}

		ShaderUniformSet& newSet = shaderUniformSets[ address ];

		int numSectors = ConfigManager::getInstance().getIntValue( "shader_num_rooms" );
		for( int i = 0; i != numSectors; i++ ) {
			auto& sectorOrigin = newSet.sectorsOrigin.emplace_back( -1 );
			auto& sectorDimensions = newSet.sectorsDimensions.emplace_back( -1 );

			if( auto optional = address->getUniform( "sectors[" + std::to_string( i ) + "].origin" ) ) {
				sectorOrigin = *optional;
				sectorDimensions = *address->getUniform( "sectors[" + std::to_string( i ) + "].dimensions" );
			}
		}

		int numSectorLevels = ConfigManager::getInstance().getIntValue( "shader_num_sector_levels" );
		for( int i = 0; i != numSectorLevels; i++ ) {
			auto& sectorMap = newSet.sectorMaps.emplace_back( -1 );

			if( auto optional = address->getUniform( "sectorMap" + std::to_string( i ) ) ) {
				sectorMap = *optional;
			}
		}

		int numSectorLights = ConfigManager::getInstance().getIntValue( "shader_num_room_lights" );
		for( int i = 0; i != numSectorLights; i++ ) {
			auto& sectorLightDirection = newSet.sectorLightsDirection.emplace_back( -1 );
			auto& sectorLightAmbient = newSet.sectorLightsAmbient.emplace_back( -1 );
			auto& sectorLightDiffuse = newSet.sectorLightsDiffuse.emplace_back( -1 );
			auto& sectorLightSpecular = newSet.sectorLightsSpecular.emplace_back( -1 );

			if( auto optional = address->getUniform( "sectorLights[" + std::to_string( i ) + "].direction" ) ) {
				sectorLightDirection = *optional;
				sectorLightAmbient = *address->getUniform( "sectorLights[" + std::to_string( i ) + "].ambient" );
				sectorLightDiffuse = *address->getUniform( "sectorLights[" + std::to_string( i ) + "].diffuse" );
				sectorLightSpecular = *address->getUniform( "sectorLights[" + std::to_string( i ) + "].specular" );
			}
		}

		return newSet;
	}

	void SectorIlluminator::send( const Shader& shader ) {
		const ShaderUniformSet& uniforms = getShaderUniformSet( &shader );

		if( dirty ) {
			refresh();

			// Texture units 1-9 are reserved for levels.
			if( textureData.size() > 8 ) {
				Log::getInstance().warn( "SectorIlluminator::send", "Cannot send > 8 sector maps to shader!" );
			}
		}

		int item = 0;
		for( const auto& [ origin, dimensions ] : levelData ) {
			shader.sendData( uniforms.sectorsOrigin[ item ], glm::vec2( origin ) );
			shader.sendData( uniforms.sectorsDimensions[ item ], dimensions );

			item++;
			if( item == 8 ) {
				break;
			}
		}

		item = 0;
		for( auto& pair : textureData ) {
			if( !pair.textureUnit ) {
				auto potential = Tools::OpenGL::getTextureUnit();
				if( !potential ) {
					Log::getInstance().error( "SectorIlluminator::send", "Couldn't get texure unit; not sending texture to shader." );
					continue;
				}

				pair.textureUnit = *potential;
			}

			if( pair.generator.valid() ) {
				Log::getInstance().debug( "SectorIlluminator::send", "Regenerating sector texture for level " + std::to_string( item ) );
				pair.texture = pair.generator.get();
			}

			if( !pair.texture ) {
				Log::getInstance().error( "SectorIlluminator::send", "Assertion failed! Texture wasn't generated for a sector." );
				continue;
			}

			glActiveTexture( GL_TEXTURE0 + *pair.textureUnit );
			glBindTexture( GL_TEXTURE_2D, pair.texture->id );
			shader.sendData( uniforms.sectorMaps[ item ], ( int ) *pair.textureUnit );

			item++;
			if( item == 8 ) {
				break;
			}
		}

		item = 0;
		for( const Sector& sector : sectors ) {
			shader.sendData( uniforms.sectorLightsDirection[ item ], sector.direction );
			shader.sendData( uniforms.sectorLightsAmbient[ item ], sector.ambient );
			shader.sendData( uniforms.sectorLightsDiffuse[ item ], sector.diffuse );
			shader.sendData( uniforms.sectorLightsSpecular[ item ], sector.specular );

			item++;
		}
	}

	/**
	 * Generate and save anything that isn't trivial to send
	 */
	void SectorIlluminator::refresh() {
		if( generatorTask.valid() ) {
			// Future is staged and is in-progress or complete
			// Check if future is ready
			if( generatorTask.wait_for( std::chrono::seconds( 0 ) ) == std::future_status::ready ) {
				// If elements were staged, we tried to add something while another operation was in progress
				// Discard the results of that and stage a new operation
				if( staging.sectors.size() || staging.levelData.size() ) {

					sectors.insert( sectors.end(), staging.sectors.begin(), staging.sectors.end() );
					levelData.insert( levelData.end(), staging.levelData.begin(), staging.levelData.end() );

					staging.sectors.clear();
					staging.levelData.clear();

					generatorTask = std::async( std::launch::async, std::bind( &SectorIlluminator::getNewTextureData, this ) );

				} else {
					for( const auto& pair : textureData ) {
						if( pair.textureUnit ) {
							Tools::OpenGL::returnTextureUnits( { *pair.textureUnit } );
						}
					}

					textureData = generatorTask.get();

					dirty = false;
				}
			}

			// Otherwise do nothing and stick with textureData
		} else {
			// Future is empty or used - Stage task to run on separate thread
			generatorTask = std::async( std::launch::async, std::bind( &SectorIlluminator::getNewTextureData, this ) );
		}
	}

	std::vector< SectorIlluminator::TextureData > SectorIlluminator::getNewTextureData() const {
		std::unique_lock< std::mutex > lock( mutex );
		std::vector< TextureData > result;

		// Create array of buffers
		// TODO: Verify all leveldata origin.z is unique
		std::vector< std::unique_ptr< float[] > > buffers;
		buffers.resize( levelData.size() );

		int resolution = 100;
		for( const auto& [ origin, dimensions ] : levelData ) {
			int height = dimensions.y * resolution;
			int width = dimensions.x * resolution;
			buffers[ origin.z ] = std::make_unique< float[] >( width * height );
		}

		// For each sector, get bounding box, and iterate each fragment in that bounding box against the sector
		int sectorIndex = 1;
		for( const Sector& sector : sectors ) {
			auto boundingBox = getBoundingBoxForSector( sector );
			int level = boundingBox.first.z;
			const auto& [ origin, dimensions ] = getLevel( level );
			int height = dimensions.y * resolution;
			int width = dimensions.x * resolution;

			boundingBox.first = glm::vec3{ std::floor( boundingBox.first.x * resolution ), std::floor( boundingBox.first.y * resolution ), boundingBox.first.z };
			boundingBox.second = glm::vec3{ std::floor( boundingBox.second.x * resolution ), std::floor( boundingBox.second.y * resolution ), boundingBox.second.z };

			int epsilon = ( boundingBox.second.x - boundingBox.first.x );

			for( int y = boundingBox.first.y; y <= boundingBox.second.y; y++ ) {
				for( int x = boundingBox.first.x; x <= boundingBox.second.x; x++ ) {
					glm::vec3 fragment{ x / ( float ) resolution, y / ( float ) resolution, level };

					// Generate needle
					std::pair< glm::vec3, glm::vec3 > needle = { fragment, fragment + glm::vec3{ epsilon, 0.0f, 0.0f } };

					// Perform point-in-polygon against sector for this fragment
					// Check all sides of this sector against the needle
					unsigned int intersectionCount = 0;
					for( const auto& side : sector.sides ) {
						if( segmentsIntersect( needle, side ) ) {
							intersectionCount++;
						}
					}

					if( ( intersectionCount % 2 ) != 0 ) {
						buffers[ level ][ ( y * width ) + x ] = sectorIndex;
					}
				}
			}

			sectorIndex++;
		}

		for( const auto& [ origin, dimensions ] : levelData ) {
			result.emplace_back( TextureData{
				nullptr, {},
				std::async( std::launch::deferred, [ width = dimensions.x * resolution, height = dimensions.y * resolution, array = std::move( buffers[ origin.z ] ) ]() {
					return std::make_unique< Texture >( glm::uvec2{ width, height }, array.get() );
				} )
			} );
		}

		return result;
	}

	const std::pair< glm::vec3, glm::uvec2 >& SectorIlluminator::getLevel( unsigned int level ) const {
		for( const auto& pair : levelData ) {
			if( level == pair.first.z ) {
				return pair;
			}
		}

		throw InvalidStateException();
	}

	std::vector< std::pair< glm::vec3, glm::vec3 > > SectorIlluminator::getSectorBoundingBoxes() const {
		std::vector< std::pair< glm::vec3, glm::vec3 > > pairs;

		for( const auto& sector : sectors ) {
			pairs.emplace_back( getBoundingBoxForSector( sector ) );
		}

		return pairs;
	}

	std::pair< glm::vec3, glm::vec3 > SectorIlluminator::getBoundingBoxForSector( const Sector& sector ) const {
		glm::vec2 min{ std::numeric_limits< float >::max(), std::numeric_limits< float >::max() };
		glm::vec2 max{ std::numeric_limits< float >::lowest(), std::numeric_limits< float >::lowest() };

		// zero-length sectors are invalid
		for( const auto& lineSegment : sector.sides ) {
			min.x = std::min( lineSegment.first.x, min.x );
			min.x = std::min( lineSegment.second.x, min.x );

			min.y = std::min( lineSegment.first.y, min.y );
			min.y = std::min( lineSegment.second.y, min.y );

			max.x = std::max( lineSegment.first.x, max.x );
			max.x = std::max( lineSegment.second.x, max.x );

			max.y = std::max( lineSegment.first.y, max.y );
			max.y = std::max( lineSegment.second.y, max.y );
		}

		return { glm::vec3{ min.x, min.y, sector.sides.front().first.z }, glm::vec3{ max.x, max.y, sector.sides.front().first.z } };
	}

	void SectorIlluminator::insert( const Sector& value ) {
		std::unique_lock< std::mutex > lock( mutex, std::defer_lock );
		if( lock.try_lock() ) {
			dirty = true;
			sectors.emplace_back( value );
		} else {
			// Operation in progress - stage it instead
			staging.sectors.emplace_back( value );
		}
	}

	void SectorIlluminator::setLevelData( const glm::vec3& topLeft, const glm::uvec2& dimensions ) {
		std::unique_lock< std::mutex > lock( mutex, std::defer_lock );
		if( lock.try_lock() ) {
			dirty = true;
			levelData.emplace_back( topLeft, dimensions );
		} else {
			// Operation in progress - stage it instead
			staging.levelData.emplace_back( topLeft, dimensions );
		}
	}

}