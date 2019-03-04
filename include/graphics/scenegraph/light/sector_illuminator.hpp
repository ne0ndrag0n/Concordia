#ifndef SG_SECTOR_ILLUMINATOR
#define SG_SECTOR_ILLUMINATOR

#include "graphics/scenegraph/illuminator.hpp"
#include <glm/glm.hpp>
#include <utility>
#include <vector>

namespace BlueBear::Graphics::SceneGraph::Light {

	class SectorIlluminator : public Illuminator {
	public:
		using Sector = std::vector< std::pair< glm::vec3, glm::vec3 > >;
		SectorIlluminator( const std::vector< Sector >& sectors );

	private:
		std::vector< Sector > sectors;

	public:
		void updateSectors( const std::vector< Sector >& sectors );
		void send() override;
	};

}

#endif