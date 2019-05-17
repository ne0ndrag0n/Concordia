#ifndef LIGHTMAP_MANAGER
#define LIGHTMAP_MANAGER

#include "models/room.hpp"
#include "graphics/texture.hpp"
#include <vector>
#include <memory>

namespace BlueBear::Graphics{ class Shader; }
namespace BlueBear::Graphics::SceneGraph::Light {

	class LightmapManager {
		std::vector< std::vector< Models::Room > > roomLevels;
		std::shared_ptr< Graphics::Texture > generatedRoomData;

	public:
		void setRooms( const std::vector< std::vector< Models::Room > >& roomLevels );

		void calculateLightmaps();

		void send( const Shader& shader );
	};

}

#endif