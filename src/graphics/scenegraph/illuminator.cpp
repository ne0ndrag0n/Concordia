#include "graphics/scenegraph/illuminator.hpp"
#include "eventmanager.hpp"

namespace BlueBear::Graphics::SceneGraph {

	Illuminator::Illuminator() {
        eventManager.SHADER_CHANGE.listen( this, [ & ]() { send(); } );
	}

	Illuminator::~Illuminator() {
      eventManager.SHADER_CHANGE.stopListening( this );
	}

}