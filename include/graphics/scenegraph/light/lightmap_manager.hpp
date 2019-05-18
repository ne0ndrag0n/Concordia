#ifndef LIGHTMAP_MANAGER
#define LIGHTMAP_MANAGER

#include "graphics/scenegraph/light/directionallight.hpp"
#include "containers/bounded_object.hpp"
#include "models/room.hpp"
#include "graphics/texture.hpp"
#include "geometry/linesegment.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

#define LIGHTMAP_SECTOR_RESOLUTION 100
#define LIGHTMAP_SECTOR_STEP	   0.01f

namespace BlueBear::Graphics{ class Shader; }
namespace BlueBear::Graphics::SceneGraph::Light {

	class LightmapManager {
		struct ShaderRoom {
			glm::vec2 lowerLeft;
			glm::vec2 upperRight;
			glm::ivec2 mapLocation;
			int level = -1;
			std::unique_ptr< float[] > mapData;
		};

		std::vector< std::vector< Models::Room > > roomLevels;
		DirectionalLight outdoorLight;

		std::shared_ptr< Graphics::Texture > generatedRoomData;
		std::vector< const DirectionalLight* > generatedLightList;

		std::vector< Geometry::LineSegment< glm::vec2 > > getEdges( const Models::Room& room );
		ShaderRoom getFragmentData( const Models::Room& room, int level, int lightIndex );
		std::vector< Containers::BoundedObject< float* > > getBoundedObjects( const std::vector< ShaderRoom >& shaderRooms );

	public:
		LightmapManager();

		void setRooms( const std::vector< std::vector< Models::Room > >& roomLevels );

		void calculateLightmaps();

		void send( const Shader& shader );
	};

}

#endif