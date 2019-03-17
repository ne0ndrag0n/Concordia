#ifndef SG_SECTOR_ILLUMINATOR
#define SG_SECTOR_ILLUMINATOR

#include "graphics/scenegraph/illuminator.hpp"
#include "graphics/texture.hpp"
#include <glm/glm.hpp>
#include <utility>
#include <map>
#include <vector>
#include <tuple>
#include <optional>

namespace BlueBear::Graphics::SceneGraph::Light {

	class SectorIlluminator : public Illuminator {
	public:
		struct Sector {
			glm::vec3 direction;
			glm::vec3 ambient;
			glm::vec3 diffuse;
			glm::vec3 specular;
			std::vector< std::pair< glm::vec3, glm::vec3 > > sides;
		};

		SectorIlluminator() = default;
		~SectorIlluminator();

	private:
		bool dirty = true;
		std::vector< Sector > sectors;
		// upper left corner and max dimensions
		std::map< unsigned int, std::pair< glm::vec3, glm::uvec2 > > levelData;
		std::map< unsigned int, std::pair< std::unique_ptr< Texture >, std::optional< unsigned int > > > textureData;

		void refresh();
		std::vector< std::pair< glm::vec3, glm::vec3 > > getSectorBoundingBoxes();

	public:
		void insert( const Sector& value );
		void setLevelData( const glm::vec3& topLeft, const glm::uvec2& dimensions );
		void send() override;
	};

}

#endif