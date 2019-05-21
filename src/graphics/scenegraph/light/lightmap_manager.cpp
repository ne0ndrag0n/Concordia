#include "graphics/scenegraph/light/lightmap_manager.hpp"
#include "geometry/methods.hpp"
#include "configmanager.hpp"
#include "tools/opengl.hpp"
#include "log.hpp"
#include <GL/glew.h>
#include <glm/gtx/string_cast.hpp>

namespace BlueBear::Graphics::SceneGraph::Light {

	static const glm::vec2 LIGHTMAP_SECTOR_MULTIPLIER{ LIGHTMAP_SECTOR_RESOLUTION, LIGHTMAP_SECTOR_RESOLUTION };

	LightmapManager::LightmapManager() : outdoorLight( { 0.5, 0.5, -0.1 }, { 0.6, 0.6, 0.6 }, { 1.0, 1.0, 1.0 }, { 0.1, 0.1, 0.1 } ) {
		Shader::SHADER_CHANGE.listen( this, std::bind( &LightmapManager::send, this, std::placeholders::_1 ) );
	}

	LightmapManager::~LightmapManager() {
		Shader::SHADER_CHANGE.stopListening( this );

		if( claimedTextureUnit ) {
			Tools::OpenGL::returnTextureUnits( { *claimedTextureUnit } );
		}
	}

	void LightmapManager::setRooms( const std::vector< std::vector< Models::Room > >& roomLevels ) {
		this->roomLevels = roomLevels;
	}

	std::vector< Geometry::LineSegment< glm::vec2 > > LightmapManager::getEdges( const Models::Room& room ) {
		// ( target + size ) % size
		std::vector< Geometry::LineSegment< glm::vec2 > > edges;

		auto points = room.getPoints();
		int pointSize = points.size();

		for( int i = 0; i != pointSize; i++ ) {
			edges.emplace_back( Geometry::LineSegment< glm::vec2 >{
				points[ ( i + pointSize ) % pointSize ],
				points[ ( ( i + 1 ) + pointSize ) % pointSize ]
			} );
		}

		return edges;
	}

	std::vector< Containers::BoundedObject< LightmapManager::ShaderRoom* > > LightmapManager::getBoundedObjects( std::vector< LightmapManager::ShaderRoom >& shaderRooms ) {
		std::vector< Containers::BoundedObject< ShaderRoom* > > result;

		for( auto& shaderRoom : shaderRooms ) {
			glm::vec2 start{ shaderRoom.lowerLeft.x, shaderRoom.upperRight.y };
			glm::vec2 end{ shaderRoom.upperRight.x, shaderRoom.lowerLeft.y };

			glm::ivec2 dims{
				( ( end.x - start.x ) * LIGHTMAP_SECTOR_RESOLUTION ),
				( ( start.y - end.y ) * LIGHTMAP_SECTOR_RESOLUTION )
			};

			result.emplace_back( Containers::BoundedObject< ShaderRoom* >{ dims.x, dims.y, &shaderRoom } );
		}

		return result;
	}

	void LightmapManager::setTexture( const std::vector< Containers::PackedCell< LightmapManager::ShaderRoom* > >& packedCells ) {
		// Get the dimensions of the board
		glm::ivec2 totalDimensions;
		for( const auto& cell : packedCells ) {
			totalDimensions.x += cell.width;
			totalDimensions.y += cell.height;
		}

		// Place all the submaps in the map
		std::unique_ptr< float[] > data = std::make_unique< float[] >( totalDimensions.x * totalDimensions.y );
		for( const auto& cell : packedCells ) {
			ShaderRoom* room = *cell.object;
			room->mapLocation = glm::ivec2{ cell.x, totalDimensions.y - ( cell.y + cell.height ) };

			for( int y = 0; y != cell.height; y++ ) {
				for( int x = 0; x != cell.width; x++ ) {
					glm::ivec2 mapCoordinates = { cell.x + x, totalDimensions.y - ( cell.y + y ) };

					data[ ( mapCoordinates.y * totalDimensions.y ) + mapCoordinates.x ] = room->mapData[ ( y * cell.width ) + x ];
				}
			}
		}

		generatedRoomData.emplace( totalDimensions, data.get() );
	}

	LightmapManager::ShaderRoom LightmapManager::getFragmentData( const Models::Room& room, int level, int lightIndex ) {
		ShaderRoom result;
		result.level = level;

		// Find lower left and upper right corner
		glm::vec2 min{ std::numeric_limits< float >::max(), std::numeric_limits< float >::max() };
		glm::vec2 max{ std::numeric_limits< float >::lowest(), std::numeric_limits< float >::lowest() };

		for( const glm::vec2& point : room.getPoints() ) {
			min.x = std::min( min.x, point.x );
			min.y = std::min( min.y, point.y );

			max.x = std::max( max.x, point.x );
			max.y = std::max( max.y, point.y );
		}

		result.lowerLeft = min;
		result.upperRight = max;

		// Build the fragment list
		glm::ivec2 start{ std::floor( result.lowerLeft.x * LIGHTMAP_SECTOR_RESOLUTION ), std::floor( result.upperRight.y * LIGHTMAP_SECTOR_RESOLUTION ) };
		glm::ivec2 end{ std::floor( result.upperRight.x * LIGHTMAP_SECTOR_RESOLUTION ), std::floor( result.lowerLeft.y * LIGHTMAP_SECTOR_RESOLUTION ) };

		result.mapData = std::make_unique< float[] >(
			( ( end.x - start.x ) ) *
			( ( start.y - end.y ) )
		);

		auto edges = getEdges( room );
		int arrayWidth = ( end.x - start.x );

		for( int y = start.y; y > end.y; y-- ) {
			for( int x = start.x; x < end.x; x++ ) {
				glm::vec2 asFloat{ x / ( float ) LIGHTMAP_SECTOR_RESOLUTION, y / ( float ) LIGHTMAP_SECTOR_RESOLUTION };

				Geometry::LineSegment< glm::vec2 > needle{ { asFloat.x, asFloat.y }, { asFloat.x + arrayWidth, asFloat.y } };

				unsigned int intersections = 0;
				for( const auto& edge : edges ) {
					if( Geometry::segmentsIntersect( needle, edge ) ) {
						intersections++;
					}
				}

				if( ( intersections % 2 ) != 0 ) {
					glm::ivec2 indices{ ( x - start.x ), ( start.y - y ) };

					result.mapData[ ( indices.y * arrayWidth ) + indices.x ] = lightIndex;
				}
			}
		}

		return result;
	}

	/**
	 * This should be called any time rooms are modified. Room nodes are immutable, so entire levels will be resent after user does something like modify a wall.
	 */
	void LightmapManager::calculateLightmaps() {
		generatedRoomData.reset();
		generatedRooms.clear();
		generatedLightList.clear();

		generatedLightList.emplace_back( &outdoorLight );

		int level = 0;
		for( const auto& roomLevel : roomLevels ) {
			for( const auto& room : roomLevel ) {
				generatedLightList.emplace_back( &room.getBackgroundLight() );
				generatedRooms.emplace_back( getFragmentData( room, level, generatedLightList.size() - 1 ) );
			}

			level++;
		}

		static int textureWidth = ConfigManager::getInstance().getIntValue( "shader_room_map_min_width" );
		static int textureHeight = ConfigManager::getInstance().getIntValue( "shader_room_map_min_height" );

		setTexture( Containers::packCells( getBoundedObjects( generatedRooms ), textureWidth, textureHeight ) );
	}

	void LightmapManager::send( const Shader& shader ) {
		const UniformBundle& bundle = uniforms.getUniforms( shader );

		for( int i = 0; i != generatedLightList.size(); i++ ) {
			shader.sendData( bundle.directionalLightsDirection[ i ], generatedLightList[ i ]->getDirection() );
			shader.sendData( bundle.directionalLightsAmbient[ i ], generatedLightList[ i ]->getAmbient() );
			shader.sendData( bundle.directionalLightsDiffuse[ i ], generatedLightList[ i ]->getDiffuse() );
			shader.sendData( bundle.directionalLightsSpecular[ i ], generatedLightList[ i ]->getSpecular() );
		}

		for( int i = 0; i != generatedRooms.size(); i++ ) {
			shader.sendData( bundle.roomsLowerLeft[ i ], generatedRooms[ i ].lowerLeft );
			shader.sendData( bundle.roomsUpperRight[ i ], generatedRooms[ i ].upperRight );
			shader.sendData2i( bundle.roomsMapLocation[ i ], generatedRooms[ i ].mapLocation );
			shader.sendData( bundle.roomsLevel[ i ], generatedRooms[ i ].level );
		}

		int lastUnsetRoom = generatedRooms.size();
		int numRoomSlots  = bundle.roomsLowerLeft.size() - lastUnsetRoom;
		for( int i = 0; i != numRoomSlots; i++ ) {
			shader.sendData( bundle.roomsLevel[ lastUnsetRoom + i ], ( int ) -1 );
		}

		if( generatedRoomData ) {
			if( !claimedTextureUnit ) {
				claimedTextureUnit = Tools::OpenGL::getTextureUnit();
				if ( !claimedTextureUnit ) {
					Log::getInstance().error( "LightmapManager::send", "Failed to obtain texture unit!" );
					return;
				}
			}

			glActiveTexture( GL_TEXTURE0 + *claimedTextureUnit );
			glBindTexture( GL_TEXTURE_2D, generatedRoomData->id );
			shader.sendData( bundle.roomData, ( int ) *claimedTextureUnit );
		}
	}

	LightmapManager::UniformBundle::UniformBundle( const Shader& shader ) {
		static int numLights = ConfigManager::getInstance().getIntValue( "shader_max_lights" );
		static int numRooms = ConfigManager::getInstance().getIntValue( "shader_max_rooms" );

		for( int i = 0; i != numLights; i++ ) {
			directionalLightsDirection.emplace_back( shader.getUniform( "directionalLights[" + std::to_string( i ) + "].direction" ) );
			directionalLightsAmbient.emplace_back( shader.getUniform( "directionalLights[" + std::to_string( i ) + "].ambient" ) );
			directionalLightsDiffuse.emplace_back( shader.getUniform( "directionalLights[" + std::to_string( i ) + "].diffuse" ) );
			directionalLightsSpecular.emplace_back( shader.getUniform( "directionalLights[" + std::to_string( i ) + "].specular" ) );
		}

		for( int i = 0; i != numRooms; i++ ) {
			roomsLowerLeft.emplace_back( shader.getUniform( "rooms[" + std::to_string( i ) + "].lowerLeft" ) );
			roomsUpperRight.emplace_back( shader.getUniform( "rooms[" + std::to_string( i ) + "].upperRight" ) );
			roomsMapLocation.emplace_back( shader.getUniform( "rooms[" + std::to_string( i ) + "].mapLocation" ) );
			roomsLevel.emplace_back( shader.getUniform( "rooms[" + std::to_string( i ) + "].level" ) );
		}

		roomData = shader.getUniform( "roomData" );
	}

}