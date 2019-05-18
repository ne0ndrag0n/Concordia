#include "graphics/scenegraph/light/lightmap_manager.hpp"
#include "containers/packed_cell.hpp"
#include "geometry/methods.hpp"
#include "configmanager.hpp"
#include "log.hpp"
#include <glm/gtx/string_cast.hpp>

namespace BlueBear::Graphics::SceneGraph::Light {

	// DEBUG - TEST code
	/*
	Log::getInstance().debug( "LightmapManager::getFragmentData", "Room: (dimensions) " + glm::to_string( dimensions ) );
	for( const glm::vec2& point : room.getPoints() ) {
		Log::getInstance().debug( "LightmapManager::getFragmentData", glm::to_string( point ) );
	}
	*/

	static const glm::vec2 LIGHTMAP_SECTOR_MULTIPLIER{ LIGHTMAP_SECTOR_RESOLUTION, LIGHTMAP_SECTOR_RESOLUTION };

	LightmapManager::LightmapManager() : outdoorLight( { 0.5, 0.5, -0.1 }, { 0.6, 0.6, 0.6 }, { 1.0, 1.0, 1.0 }, { 0.1, 0.1, 0.1 } ) {}

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

	std::vector< Containers::BoundedObject< float* > > LightmapManager::getBoundedObjects( const std::vector< LightmapManager::ShaderRoom >& shaderRooms ) {
		std::vector< Containers::BoundedObject< float* > > result;

		for( const auto& shaderRoom : shaderRooms ) {
			glm::vec2 start{ shaderRoom.lowerLeft.x, shaderRoom.upperRight.y };
			glm::vec2 end{ shaderRoom.upperRight.x, shaderRoom.lowerLeft.y };

			glm::ivec2 dims{
				( ( end.x - start.x ) * LIGHTMAP_SECTOR_RESOLUTION ),
				( ( start.y - end.y ) * LIGHTMAP_SECTOR_RESOLUTION )
			};

			result.emplace_back( Containers::BoundedObject< float* >{ dims.x, dims.y, shaderRoom.mapData.get() } );
		}

		return result;
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
		glm::vec2 start{ result.lowerLeft.x, result.upperRight.y };
		glm::vec2 end{ result.upperRight.x, result.lowerLeft.y };

		result.mapData = std::make_unique< float[] >(
			( ( end.x - start.x ) * LIGHTMAP_SECTOR_RESOLUTION ) *
			( ( start.y - end.y ) * LIGHTMAP_SECTOR_RESOLUTION )
		);

		auto edges = getEdges( room );
		float epsilon = end.x - start.x;
		int arrayWidth = ( end.x - start.x ) * LIGHTMAP_SECTOR_RESOLUTION;

		for( float y = start.y; y >= end.y; y -= LIGHTMAP_SECTOR_STEP ) {
			for( float x = start.x; x <= end.x; x += LIGHTMAP_SECTOR_STEP ) {
				Geometry::LineSegment< glm::vec2 > needle{ { x, y }, { x + epsilon, y } };

				unsigned int intersections = 0;
				for( const auto& edge : edges ) {
					if( Geometry::segmentsIntersect( needle, edge ) ) {
						intersections++;
					}
				}

				if( ( intersections % 2 ) != 0 ) {
					glm::ivec2 indices{ ( x - start.x ) * LIGHTMAP_SECTOR_RESOLUTION, ( start.y - y ) * LIGHTMAP_SECTOR_RESOLUTION };

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
		generatedRoomData = nullptr;
		generatedLightList.clear();

		generatedLightList.emplace_back( &outdoorLight );

		std::vector< ShaderRoom > shaderRooms;
		int level = 0;
		for( const auto& roomLevel : roomLevels ) {
			for( const auto& room : roomLevel ) {
				generatedLightList.emplace_back( &room.getBackgroundLight() );
				shaderRooms.emplace_back( getFragmentData( room, level, generatedLightList.size() - 1 ) );
			}

			level++;
		}

		static int textureWidth = ConfigManager::getInstance().getIntValue( "shader_room_map_min_width" );
		static int textureHeight = ConfigManager::getInstance().getIntValue( "shader_room_map_min_height" );

		auto packedCells = Containers::packCells( getBoundedObjects( shaderRooms ), textureWidth, textureHeight );
	}

}