#include "graphics/scenegraph/light/lightmap_manager.hpp"
#include "log.hpp"
#include <glm/gtx/string_cast.hpp>

namespace BlueBear::Graphics::SceneGraph::Light {

	void LightmapManager::setRooms( const std::vector< std::vector< Models::Room > >& roomLevels ) {
		this->roomLevels = roomLevels;
	}

	/**
	 * This should be called any time rooms are modified. Room nodes are immutable, so entire levels will be resent after user does something like modify a wall.
	 */
	void LightmapManager::calculateLightmaps() {
		// Test code that prints the rooms
		for( const auto& roomLevel : roomLevels ) {
			for( const auto& room : roomLevel ) {

				Log::getInstance().debug( "LightmapManager::calculateLightmaps", "Room:" );
				for( const glm::vec2& point : room.getPoints() ) {
					Log::getInstance().debug( "LightmapManager::calculateLightmaps", glm::to_string( point ) );
				}

			}
		}
	}

}