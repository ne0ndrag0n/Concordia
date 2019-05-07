#include "graphics/scenegraph/illuminator.hpp"
#include "graphics/shader.hpp"
#include "eventmanager.hpp"

namespace BlueBear::Graphics::SceneGraph {

	Illuminator::Illuminator() {
		Graphics::Shader::SHADER_CHANGE.listen( this, [ this ]( const Shader& shader ) { send( shader ); } );
	}

	Illuminator::~Illuminator() {
		Graphics::Shader::SHADER_CHANGE.stopListening( this );
	}

}