#include "graphics/scenegraph/light/sector_illuminator.hpp"
#include "tools/opengl.hpp"

namespace BlueBear::Graphics::SceneGraph::Light {

	SectorIlluminator::SectorIlluminator( const std::vector< SectorIlluminator::Sector >& sectors ) : Illuminator::Illuminator() {
		updateSectors( sectors );
	}

	void SectorIlluminator::send() {
		Tools::OpenGL::setUniform( "sectorIlluminator.numSectors", ( unsigned int ) sectors.size() );
	}

	void SectorIlluminator::updateSectors( const std::vector< SectorIlluminator::Sector >& sectors ) {
		this->sectors = sectors;
	}

}