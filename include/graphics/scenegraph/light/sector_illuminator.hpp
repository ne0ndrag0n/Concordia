#ifndef SG_SECTOR_ILLUMINATOR
#define SG_SECTOR_ILLUMINATOR

#include "exceptions/genexc.hpp"
#include "graphics/scenegraph/illuminator.hpp"
#include "graphics/texture.hpp"
#include "graphics/shader.hpp"
#include <glm/glm.hpp>
#include <utility>
#include <map>
#include <vector>
#include <tuple>
#include <optional>
#include <future>
#include <unordered_map>

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

		EXCEPTION_TYPE( InvalidStateException, "Invalid state encountered!" );
	private:
		struct TextureData {
			std::unique_ptr< Texture > texture = nullptr;
			std::optional< unsigned int > textureUnit;
			std::future< std::unique_ptr< Texture > > generator;
		};

		struct ShaderUniformSet {
			std::vector< Shader::Uniform > levelsOrigin;
			std::vector< Shader::Uniform > levelsDimensions;
			std::vector< Shader::Uniform > sectorMaps;
			std::vector< Shader::Uniform > sectorLightsDirection;
			std::vector< Shader::Uniform > sectorLightsAmbient;
			std::vector< Shader::Uniform > sectorLightsDiffuse;
			std::vector< Shader::Uniform > sectorLightsSpecular;
		};

		bool dirty = true;
		struct {
			std::vector< Sector > sectors;
			std::vector< std::pair< glm::vec3, glm::uvec2 > > levelData;
		} staging;
		mutable std::mutex mutex;
		std::future< std::vector< TextureData > > generatorTask;

		std::vector< Sector > sectors;
		std::vector< std::pair< glm::vec3, glm::uvec2 > > levelData;

		std::vector< TextureData > textureData;

		std::unordered_map< const void*, ShaderUniformSet > shaderUniformSets;

		const ShaderUniformSet& getShaderUniformSet( const Shader* address );
		void refresh();

		const std::pair< glm::vec3, glm::uvec2 >& getLevel( unsigned int level ) const;
		std::vector< std::pair< glm::vec3, glm::vec3 > > getSectorBoundingBoxes() const;
		std::pair< glm::vec3, glm::vec3 > getBoundingBoxForSector( const Sector& sector ) const;
		std::vector< TextureData > getNewTextureData() const;

	public:
		void insert( const Sector& value );
		void setLevelData( const glm::vec3& topLeft, const glm::uvec2& dimensions );
		void send( const Shader& shader ) override;
	};

}

#endif