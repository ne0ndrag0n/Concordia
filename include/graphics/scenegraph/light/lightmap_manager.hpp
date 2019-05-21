#ifndef LIGHTMAP_MANAGER
#define LIGHTMAP_MANAGER

#include "graphics/scenegraph/light/directionallight.hpp"
#include "graphics/uniform_cache.hpp"
#include "containers/bounded_object.hpp"
#include "containers/packed_cell.hpp"
#include "models/room.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"
#include "geometry/linesegment.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <optional>

#define LIGHTMAP_SECTOR_RESOLUTION 100

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
		struct UniformBundle {
			std::vector< Shader::Uniform > directionalLightsDirection;
			std::vector< Shader::Uniform > directionalLightsAmbient;
			std::vector< Shader::Uniform > directionalLightsDiffuse;
			std::vector< Shader::Uniform > directionalLightsSpecular;
			std::vector< Shader::Uniform > roomsLowerLeft;
			std::vector< Shader::Uniform > roomsUpperRight;
			std::vector< Shader::Uniform > roomsMapLocation;
			std::vector< Shader::Uniform > roomsLevel;
			Shader::Uniform roomData;

			UniformBundle() = default;
			UniformBundle( const Shader& shader );
		};

		std::vector< std::vector< Models::Room > > roomLevels;
		DirectionalLight outdoorLight;

		UniformCache< UniformBundle > uniforms;
		std::optional< Graphics::Texture > generatedRoomData;
		std::vector< ShaderRoom > generatedRooms;
		std::vector< const DirectionalLight* > generatedLightList;
		std::optional< unsigned int > claimedTextureUnit;

		std::vector< Geometry::LineSegment< glm::vec2 > > getEdges( const Models::Room& room );
		ShaderRoom getFragmentData( const Models::Room& room, int level, int lightIndex );
		std::vector< Containers::BoundedObject< ShaderRoom* > > getBoundedObjects( std::vector< ShaderRoom >& shaderRooms );
		void setTexture( const Containers::PackedCellMap< ShaderRoom* >& packedCells );

	public:
		LightmapManager();
		~LightmapManager();

		void setRooms( const std::vector< std::vector< Models::Room > >& roomLevels );

		void calculateLightmaps();

		void send( const Shader& shader );
	};

}

#endif