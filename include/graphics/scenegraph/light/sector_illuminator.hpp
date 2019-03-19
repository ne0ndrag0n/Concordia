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
#include <future>

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
		struct TextureData {
			std::unique_ptr< Texture > texture = nullptr;
			std::optional< unsigned int > textureUnit;
			std::future< std::unique_ptr< Texture > > generator;
		};

		bool dirty = true;
		struct {
			std::vector< Sector > sectors;
			std::map< unsigned int, std::pair< glm::vec3, glm::uvec2 > > levelData;
		} staging;
		mutable std::mutex mutex;
		std::future< std::vector< TextureData > > generatorTask;

		std::vector< Sector > sectors;
		std::map< unsigned int, std::pair< glm::vec3, glm::uvec2 > > levelData;

		std::vector< TextureData > textureData;

		void refresh();
		std::vector< std::pair< glm::vec3, glm::vec3 > > getSectorBoundingBoxes() const;
		std::vector< TextureData > getNewTextureData() const;

	public:
		void insert( const Sector& value );
		void setLevelData( const glm::vec3& topLeft, const glm::uvec2& dimensions );
		void send() override;
	};

}

#endif