#ifndef SG_SECTOR_ILLUMINATOR
#define SG_SECTOR_ILLUMINATOR

#include "graphics/scenegraph/illuminator.hpp"
#include <glm/glm.hpp>
#include <utility>
#include <map>
#include <vector>

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
		SectorIlluminator( const std::vector< Sector >& sectors );

	private:
		std::vector< Sector > sectors;
		std::map< float, glm::vec3 > origins;

	public:
		void updateSectors( const std::vector< Sector >& sectors );
		void insert( const Sector& value );
		void setOrigin( float level, const glm::vec3& topLeft );
		Sector* getSector();
		void send() override;
	};

}

#endif